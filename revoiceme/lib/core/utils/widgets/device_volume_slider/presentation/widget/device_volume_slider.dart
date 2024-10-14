import "dart:async";

import "package:flutter/material.dart";
import "package:revoiceme/core/utils/widgets/voice_changer_slider/presentation/widget/voice_changer_slider.dart";
import "package:volume_controller/volume_controller.dart";

/// A slider for the device volume.
///
/// The [DeviceVolumeSlider] provides bidrectional synchronization between the device volume and the slider value.
///
/// {@category Widget}
/// {@category Global}
class DeviceVolumeSlider extends StatefulWidget {
  /// Creates a new [DeviceVolumeSlider] instance.
  const DeviceVolumeSlider({
    super.key,
  });

  @override
  State<DeviceVolumeSlider> createState() => _DeviceVolumeSliderState();

  /// Cleans up the volume controller listener to free resources.
  /// Pass this method as the 'cleanup' parameter for a RevoiceMeDrawer.
  /// There is always either one or none VolumeController in an app globally registered.
  /// This code is not in the 'dispose' because navigating to a new screen calls the 'dispose' method
  /// of the current screen AFTER the 'initState' of the new screen.
  /// Because all (de-)registrations happen globally and idempotently, the new screen would end up with no VolumeController listener.
  static void cleanup() {
    VolumeController().removeListener();
  }
}

class _DeviceVolumeSliderState extends State<DeviceVolumeSlider> {
  /// Listens to changes in the device volume. Syncs system controls with in app volume controls.
  ValueNotifier<double> deviceVolume = ValueNotifier<double>(0.0);

  static const double minVolume = 0.0;
  static const double maxVolume = 1.0;

  @override
  void initState() {
    VolumeController().listener((double volume) => deviceVolume.value = volume);
    // don't show because its popin animation plays after every value change
    VolumeController().showSystemUI = false;
    WidgetsBinding.instance.addPostFrameCallback((_) async {
      await _initVolume();
    });
    super.initState();
  }

  @override
  void dispose() {
    deviceVolume.dispose();
    super.dispose();
  }

  /// Set the initial volume of the device in the [deviceVolume] value notifier.
  Future<void> _initVolume() async {
    deviceVolume.value = await VolumeController().getVolume();
  }

  // TODO(L483): Implement a way to infer the steps automatically based on the device.
  /// The step size of the slider.
  /// 15 steps is the default step count for the (development) device's volume slider.
  double delta() => maxVolume / 15;

  /// Handles changes in the device volume and keeps the volume inside the device bounds.
  Future<void> _onChanged(double volume) async {
    if (volume == deviceVolume.value) return;
    if (volume >= maxVolume) {
      await _showMaxVolumeWarning();
    } else if (volume >= minVolume) {
      VolumeController().setVolume(volume);
    }
  }

  /// Shows a warning dialog and a helping message when the user
  /// tries to set the volume to a value above the maximum.
  Future<void> _showMaxVolumeWarning() async {
    await showDialog<void>(
      context: context,
      builder: (BuildContext context) => AlertDialog(
        title: const Text("Maximale Lautstärke erreicht."),
        content: const Text(
          "Stellen Sie sicher, dass das Ausgabegerät angeschlossen und eingeschalten ist.",
        ),
        actions: <Widget>[
          TextButton(
            onPressed: Navigator.of(context).pop,
            child: const Text("Ok"),
          ),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) => ValueListenableBuilder<double>(
        valueListenable: deviceVolume,
        builder: (BuildContext context, double deviceVolume, Widget? child) =>
            VoiceChangerSlider(
          title: "Lautstärke",
          value: deviceVolume,
          min: minVolume,
          max: maxVolume,
          delta: delta(),
          onChanged: _onChanged,
        ),
      );
}
