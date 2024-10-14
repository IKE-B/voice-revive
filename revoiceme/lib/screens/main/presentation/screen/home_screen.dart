import "package:flutter/material.dart";
import "package:revoiceme/core/utils/widgets/device_volume_slider/presentation/widget/device_volume_slider.dart";
import "package:revoiceme/core/utils/widgets/play_button/presentation/widget/play_button.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_appbar/presentation/widget/revoiceme_appbar.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_drawer/presentation/widget/revoiceme_drawer.dart";
import "package:revoiceme/core/utils/widgets/small_padder/presentation/widget/small_padder.dart";
import "package:revoiceme/core/utils/widgets/voice_changer_slider/presentation/widget/voice_changer_slider.dart";

// TODO(L483): Integrate the app with the real voice changing feature.

/// The home screen of the ReVoiceMe app.
///
/// {@category Screen}
class HomeScreen extends StatefulWidget {
  /// Creates a new [HomeScreen] instance.
  const HomeScreen({
    super.key,
  });

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  /// Listens to changes in the pitch parameter for the voice changer. Only a stub for now.
  ValueNotifier<double> pitch = ValueNotifier<double>(1.5);

  @override
  void dispose() {
    // cleanup value notifiers
    pitch.dispose();
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
              const PlayButton(size: 80),
              const SmallPadder(),
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
            ],
          ),
        ),
      );
}
