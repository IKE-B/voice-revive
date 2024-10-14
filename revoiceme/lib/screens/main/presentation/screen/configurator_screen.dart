import "dart:async";
import "package:flutter/material.dart";
import "package:revoiceme/core/utils/widgets/device_volume_slider/presentation/widget/device_volume_slider.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_appbar/presentation/widget/revoiceme_appbar.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_drawer/presentation/widget/revoiceme_drawer.dart";
import "package:revoiceme/core/utils/widgets/voice_changer_slider/presentation/widget/voice_changer_slider.dart";

// TODO(L483): Integrate the app with the real voice changing feature.

/// The screen for fine-granular configuration of the voice changing.
///
/// {@category Screen}
class ConfiguratorScreen extends StatefulWidget {
  /// Creates a new [ConfiguratorScreen] instance.
  const ConfiguratorScreen({
    super.key,
  });

  @override
  State<ConfiguratorScreen> createState() => _ConfiguratorScreenState();
}

class _ConfiguratorScreenState extends State<ConfiguratorScreen> {
  /// Listens to changes in the pitch parameter for the voice changer. Only a stub for now.
  ValueNotifier<double> pitch = ValueNotifier<double>(1.5);

  /// Listens to changes in the C parameter for the voice changer. Only a stub for now.
  ValueNotifier<double> parameterC = ValueNotifier<double>(0);

  /// Listens to changes in the D parameter for the voice changer. Only a stub for now.
  ValueNotifier<double> parameterD = ValueNotifier<double>(10);

  @override
  Future<void> dispose() async {
    // cleanup value notifiers
    pitch.dispose();
    parameterC.dispose();
    parameterD.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) => Scaffold(
        appBar: const ReVoiceMeAppBar(),
        drawer: const ReVoiceMeDrawer(
          cleanup: DeviceVolumeSlider.cleanup,
        ),
        body: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              const DeviceVolumeSlider(),
              ValueListenableBuilder<double>(
                valueListenable: pitch,
                builder: (BuildContext context, double value, Widget? child) =>
                    VoiceChangerSlider(
                  title: "Tonhöhe",
                  value: value,
                  min: -50,
                  max: 50,
                  delta: 5.0,
                  togglable: true,
                  onChanged: (double value) {
                    debugPrint("$pitch");
                    debugPrint("$value");
                    pitch.value = value;
                  },
                ),
              ),
              ValueListenableBuilder<double>(
                valueListenable: parameterC,
                builder: (BuildContext context, double value, Widget? child) =>
                    VoiceChangerSlider(
                  title: "Parameter C",
                  value: value,
                  min: -10,
                  max: 20,
                  delta: 2.0,
                  onChanged: (double value) {
                    debugPrint("$parameterC");
                    debugPrint("$value");
                    parameterC.value = value;
                  },
                ),
              ),
              ValueListenableBuilder<double>(
                valueListenable: parameterD,
                builder: (BuildContext context, double value, Widget? child) =>
                    VoiceChangerSlider(
                  title: "Parameter D",
                  value: value,
                  min: 0,
                  max: 10,
                  delta: 2.5,
                  togglable: true,
                  onChanged: (double value) {
                    debugPrint("$parameterD");
                    debugPrint("$value");
                    parameterD.value = value;
                  },
                ),
              ),
            ],
          ),
        ),
      );
}
