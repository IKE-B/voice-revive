import "dart:ffi" as ffi;
import 'dart:io' show Platform;

typedef startModulationFunctionCPP = ffi.Void Function();
typedef startModulationFunctionDart = void Function();
typedef stopModulationFunctionCPP = ffi.Void Function();
typedef stopModulationFunctionDart = void Function();
typedef updateValuesFunctionCPP = ffi.Void Function(ffi.Float, ffi.Float, ffi.Float, ffi.Float, ffi.Float, ffi.Bool, ffi.Bool,
                                                ffi.Float, ffi.Float, ffi.Float, ffi.Float, ffi.Bool, ffi.Bool, ffi.Bool,
                                                ffi.Float, ffi.Float, ffi.Float, ffi.Float, ffi.Bool, ffi.Bool, ffi.Bool,
                                                ffi.Float, ffi.Float, ffi.Float, ffi.Float, ffi.Bool, ffi.Bool, ffi.Bool,
                                                ffi.Float, ffi.Float, ffi.Float, ffi.Float, ffi.Float, ffi.Float, ffi.Float, ffi.Float);
typedef updateValuesFunctionDart = void Function(double, double, double, double, double, bool, bool,
                                                double, double, double, double, bool, bool, bool,
                                                double, double, double, double, bool, bool, bool,
                                                double, double, double, double, bool, bool, bool,
                                                double, double, double, double, double, double, double, double);

late final startModulation, stopModulation, updateValues;
late final ffi.DynamicLibrary nativeLibrary;

void initLibrary() 
{
  nativeLibrary = Platform.isMacOS || Platform.isIOS
        ? ffi.DynamicLibrary.process() // macos and ios
        : (ffi.DynamicLibrary.open(Platform.isWindows // windows
            ? 'libAudioProcessor.dll'
            : 'libAudioProcessor.so'));
  startModulation = nativeLibrary.lookupFunction<startModulationFunctionCPP, startModulationFunctionDart>('startModulation');
  stopModulation = nativeLibrary.lookupFunction<stopModulationFunctionCPP, stopModulationFunctionDart>('stopModulation');
  updateValues = nativeLibrary.lookupFunction<updateValuesFunctionCPP, updateValuesFunctionDart>('updateValues');
}