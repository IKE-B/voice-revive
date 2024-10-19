import "package:flutter/material.dart";
import "package:revoiceme/core/utils/widgets/device_volume_slider/presentation/widget/device_volume_slider.dart";
import "package:revoiceme/core/utils/widgets/play_button/presentation/widget/play_button.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_appbar/presentation/widget/revoiceme_appbar.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_drawer/presentation/widget/revoiceme_drawer.dart";
import "package:revoiceme/core/utils/widgets/small_padder/presentation/widget/small_padder.dart";

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
  @override
  void dispose() {
    // cleanup value notifiers
    super.dispose();
  }

  @override
  Widget build(BuildContext context) => const Scaffold(
        appBar: ReVoiceMeAppBar(),
        drawer: ReVoiceMeDrawer(
          cleanup: DeviceVolumeSlider.cleanup,
        ),
        body: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              PlayButton(size: 80),
              SmallPadder(),
              SmallPadder(),
              SmallPadder(),
              DeviceVolumeSlider(),
            ],
          ),
        ),
      );
}
