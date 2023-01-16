#!/usr/bin/env python

import subprocess
import json
import time
import argparse
import sys
from pathlib import Path

PRICE = 'Price'
HEDGE = 'Hedge'
price_or_hedge = None
missing_parameters = False
outdir = None
datadir = None
execdir = None
execpath = None
marketdir = None
parser = argparse.ArgumentParser()
parser.add_argument("--toplevel", help="Top level directory for the whole structure. This option takes precedence over all others.", type=str)
parser.add_argument("--exec", help="Path to executable. Optional if --toplevel is specified.", type=str)
parser.add_argument("--datadir", help="Path to input data directory. Optional if --toplevel is specified.", type=str)
parser.add_argument("--marketdir", help="Where to look for market data. To be used only with --hedge option. If not specified, use the same directory as for --datadir", type=str)
parser.add_argument("--outdir", help="Where to write the output. Optional if --toplevel is specified.", type=str)
group = parser.add_mutually_exclusive_group()
group.add_argument("--price", help="Test the prices at time 0.", action="store_true")
group.add_argument("--hedge", help="Test the P&L.", action="store_true")
args = parser.parse_args()
if args.toplevel:
    structuredir = args.toplevel
    outdir = Path(structuredir)
    exec_folder = Path(outdir) / "Executables"
    datadir = Path(outdir) / "Tests"
else:
    if not args.exec:
        print('Missing "--exec" parameter')
        missing_parameters = True
    else:
        execpath = Path(args.exec).resolve()
    if not args.datadir:
        print('Missing "--datadir" parameter')
        missing_parameters = True
    else:
        datadir = Path(args.datadir)
    if not args.outdir:
        print('Missing "--outdir" parameter')
        missing_parameters = True
    else:
        outdir = Path(args.outdir)
    if args.price:
        price_or_hedge = PRICE
    elif args.hedge:
        price_or_hedge = HEDGE
        if args.marketdir:
            marketdir = Path(args.marketdir)
        else:
            marketdir = datadir
    else:
        print('Missing action parameter: "--price" or "--hedge"')
        missing_parameters = True
    if missing_parameters:
        sys.exit(1)


# Top level Structure to write computation results and comparison outputs
outdir_folder = outdir.resolve()
outdir_folder.mkdir(parents=True, exist_ok=True)
# Directory containing all the tests.
test_folder = datadir.resolve()
if marketdir:
    market_folder = marketdir.resolve()


def compare_prices0(expected, obtained, name='price'):
    """
    Compare the prices at time 0 using the std deviation of the MC estimator
    """
    expected_price = expected[name]
    expected_std_dev = expected[name + "StdDev"]
    obtained_price = obtained[name]
    same_interval = abs(expected_price - obtained_price) / expected_std_dev
    # computed stddevs ratios
    obtained_std_dev = obtained[name + "StdDev"]
    std_dev_ratio = obtained_std_dev / expected_std_dev
    return (same_interval, std_dev_ratio)


def compare_deltas(expected, obtained, name='delta'):
    """
    Compare the deltas at time 0 using the std deviation of the MC estimator
    """
    expected_delta = expected[name]
    obtained_delta = obtained[name]
    expected_delta_std_dev = expected[name + "StdDev"]
    obtained_delta_std_dev = obtained[name + "StdDev"]
    delta_same_interval = []
    delta_std_dev_ratio = []
    for ind, val in enumerate(expected_delta):
        delta_same_interval.append(abs(val - obtained_delta[ind]) / expected_delta_std_dev[ind])
        delta_std_dev_ratio.append(obtained_delta_std_dev[ind] / expected_delta_std_dev[ind])
    return max(delta_same_interval), max(delta_std_dev_ratio)


def compare_pnl(expected, obtained):
    obtained_pnl = obtained["finalPnL"]
    expected_pnl = expected["finalPnL"]
    pnl_difference = abs(obtained_pnl - expected_pnl)
    return pnl_difference


def compare_times(expected, obtained):
    obtained_time = obtained["time"]
    expected_time = expected["time"]
    time_difference = obtained_time / expected_time
    return time_difference

def compare_pricing_results(output_path, expected_path):
    """
    Compare the results output by the pricing at time 0
    """
    with output_path.open() as output_stream:
        with expected_path.open() as expected_stream:
            expected = json.load(expected_stream)
            obtained = json.load(output_stream)
            # is obtained price in the same interval as expected price?
            same_interval, std_dev_ratio = compare_prices0(expected, obtained)
            delta_same_interval, delta_std_dev_ratio = compare_deltas(expected, obtained)
            # computational time
            time_ratio = compare_times(expected, obtained)
            result = {
                "priceDistance":same_interval,
                "stdDevRatio": std_dev_ratio,
                "deltaComponentDistance": delta_same_interval,
                "deltaStdDevRatios": delta_std_dev_ratio,
                "timeRatio": time_ratio
                }
            return result

def compare_hedging_results(output_path, expected_path):
    """
    Compare the hedging results
    """
    with output_path.open() as output_stream:
        with expected_path.open() as expected_stream:
            expected = json.load(expected_stream)
            obtained = json.load(output_stream)
            # is obtained price in the same interval as expected price?
            same_interval, std_dev_ratio = compare_prices0(expected, obtained, 'initialPrice')
            pnl_difference = compare_pnl(expected, obtained)
            # computational time
            time_ratio = compare_times(expected, obtained)
            result = {
                "priceDistance":same_interval,
                "stdDevRatio": std_dev_ratio,
                "pnlDifference": pnl_difference,
                "timeRatio": time_ratio
                }
            return result

def run_allexec_tests(action):
    for exe in exec_folder.iterdir():
        if not exe.match('**/exec_*'):
            continue
        exe_fullpath = exec_folder / exe
        run_singleexec_test(exe_fullpath, action)


def run_singleexec_test(exe_fullpath, action):
    output_folder = outdir_folder / "Outputs" / (exe_fullpath.stem + "_output")
    output_folder.mkdir(parents=True, exist_ok=True)
    scenario_folder = output_folder / test_folder.name
    scenario_folder.mkdir(parents=True, exist_ok=True)
    for test_case in test_folder.iterdir():
        if test_case.suffix != ".dat" or test_case.match('*_market.dat'):
            continue
        outfile_path = None
        timeout = None
        proc_args = [exe_fullpath.as_posix()]
        if action == PRICE:
            outfile_path = scenario_folder / (test_case.stem + "_obtained_price.json")
            proc_args.append(test_case.as_posix())
            timeout = 60
        elif action == HEDGE:
            test_case_market = market_folder / (test_case.stem + '_market.dat')
            if not test_case_market.exists:
                print('Market file ', test_case_market.as_posix(), ' does not exist')
                continue
            proc_args.extend([test_case_market.as_posix(), test_case.as_posix()])
            outfile_path = scenario_folder / (test_case.stem + "_obtained_hedge.json")
            timeout = 300
        else:
            print("Unknown action")
            return

        print("Running ", proc_args)
        try:
            start_time = time.time()
            exec_output = subprocess.run(proc_args, stdout=subprocess.PIPE, timeout=timeout, check=True)
            end_time = time.time()
            exec_result = json.loads(exec_output.stdout.decode('utf-8'))
            dict_result = {
                "time": end_time - start_time,
            }
            dict_result.update(exec_result)
            with outfile_path.open(mode='w') as outfile:
                json.dump(dict_result, outfile, indent=4)
        except json.JSONDecodeError:
            print('Output of "', ' '.join(proc_args), '" is not of JSON type')
            print(exec_output.stdout)
        except subprocess.CalledProcessError as e:
            print('--', e)
            print('--', e.output.decode('utf-8'))
        except subprocess.TimeoutExpired:
            print('Timeout running ', proc_args)


#compare results
def run_compare_results(action):
    for output_folder in (outdir_folder / "Outputs").iterdir():
        if sum([tok[0] == '.' for tok in output_folder.relative_to(outdir_folder).parts]):
            continue
        if not output_folder.match('**/*_output'):
            continue
        exe = None
        expected_suffix = None
        obtained_suffix = None
        result_suffix = None
        compare_function = None
        if action == PRICE:
            obtained_suffix = '_obtained_price'
            expected_suffix = '_expected_price'
            result_suffix = '_result_price'
            compare_function = compare_pricing_results
        elif action == HEDGE:
            obtained_suffix = '_obtained_hedge'
            expected_suffix = '_expected_hedge'
            result_suffix = '_result_hedge'
            compare_function = compare_hedging_results
        exe = output_folder.stem.replace(obtained_suffix, '')
        result_folder = outdir_folder / "Results"
        result_folder.mkdir(parents=True, exist_ok=True)
        test_case_results = []
        for test_case in  test_folder.iterdir():
            if test_case.suffix != ".dat" or test_case.match('*_market.dat'):
                continue
            expected_file = test_case.parent / (test_case.stem + expected_suffix + '.json')
            output_file = output_folder / datadir.stem / (test_case.stem + obtained_suffix + '.json')
            if not output_file.exists():
                print('File ', output_file, ' does not exist')
                continue
            print(exe + ": comparing " + expected_file.name + " with " + output_file.name)
            comparison = compare_function(output_file, expected_file)
            test_case_comparison = {
                "testCaseName": test_case.stem,
                "comparison": comparison
                }
            test_case_results.append(test_case_comparison)
        result_file = result_folder / (exe + result_suffix + '.json')
        with result_file.open('w') as result_stream:
            json.dump(test_case_results, result_stream, indent=4)

if __name__ == "__main__":
    if execpath:
        run_singleexec_test(execpath, price_or_hedge)
    else:
        run_allexec_tests(price_or_hedge)
    run_compare_results(price_or_hedge)
