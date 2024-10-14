import "dart:async";
import "package:flutter/material.dart";
import "package:flutter_sound/flutter_sound.dart";
import "package:permission_handler/permission_handler.dart";
import "package:revoiceme/core/routers/router.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_appbar/presentation/widget/revoiceme_appbar.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_drawer/presentation/widget/revoiceme_drawer.dart";
import "package:revoiceme/core/utils/widgets/small_padder/presentation/widget/small_padder.dart";
import "package:revoiceme/core/utils/widgets/voice_changer_control/presentation/widget/voice_changer_control.dart";
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
    VolumeController().removeListener();
    deviceVolume.dispose();
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
              VoiceChangerControl(
                toggleVoiceChanging:
                    _isAmplifying ? _stopAmplifying : _startAmplifying,
                userHasControl: true,
                appliesVoiceChanging: _isAmplifying,
                size: 80,
                activeIcon: Icons.pause_rounded,
                inactiveIcon: Icons.play_arrow_rounded,
              ),
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
