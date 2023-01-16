Open cmake-gui.exe.
Select the BarrierOptionPricing folder for the location of the source code.
Select the Build folder of your choice.
Add a 'CMAKE_PREFIX_PATH' entry, set its type to 'PATH' and provide the location to libpnl.
Click on 'Configure', then 'Generate'.
Warning: CMake needs to download googletests, which can take a lot of time

Nb: if the unit tests do not compile, there is a commented line that can be modified in the corresponding CMakeLists file.

The unit tests can be run either from Visual Studio, or from the Build\BarrierOptionPricing_test\Debug folder.
The pricer can be found in the Build\SourceCode\BarrierOptionPricing\Debug folder. It takes a .dat file as an argument, one such file is provided in the BarrierFiles folder.