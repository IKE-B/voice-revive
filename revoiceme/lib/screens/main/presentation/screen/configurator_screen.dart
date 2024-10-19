import "dart:async";
import "package:flutter/material.dart";
import "package:revoiceme/core/utils/constants/juce_parameters.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";
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
  late Map<String, ValueNotifier<Object>> notifiers;

  @override
  void initState() {
    notifiers = initValueNotifiers();
    super.initState();
  }

  /// Initializes the value notifiers for the sliders, dropdowns, and checkboxes.
  Map<String, ValueNotifier<Object>> initValueNotifiers() =>
      Map<String, ValueNotifier<Object>>.fromEntries(
        <Iterable<MapEntry<String, ValueNotifier<Object>>>>[
          initCheckboxNotifiers(),
          initDropdownNotifiers(),
          initSliderNotifiers(),
        ]
            .expand(
              (Iterable<MapEntry<String, ValueNotifier<Object>>> elem) => elem,
            )
            .toList(),
      );

  /// Initializes the value notifiers for the sliders.
  /// Walks over all parameters and creates [MapEntry]s.
  /// The 'key' is the same as the key in their original [juceSliderParameters].
  /// The value is a [ValueNotifier] initialized with the corresponding default value of the parameter.
  Iterable<MapEntry<String, ValueNotifier<Object>>> initSliderNotifiers() =>
      juceSliderParameters.entries.map(
        (
          MapEntry<
                  String,
                  ({
                    double delta,
                    double init,
                    String Function(double p1) labelBuilder,
                    double max,
                    double min,
                    String title,
                    bool togglable
                  })>
              param,
        ) =>
            MapEntry<String, ValueNotifier<double>>(
          param.key,
          ValueNotifier<double>(param.value.init),
        ),
      );

  /// Initializes the value notifiers for the dropdowns.
  /// Walks over all parameters and creates [MapEntry]s.
  /// The 'key' is the same as the key in their original [juceSliderParameters].
  /// The value is a [ValueNotifier] initialized with the corresponding default value of the parameter.
  Iterable<MapEntry<String, ValueNotifier<Object>>> initDropdownNotifiers() =>
      juceDropdownParameters.entries.map(
        (
          MapEntry<String, ({double init, String title, List<double> values})>
              param,
        ) =>
            MapEntry<String, ValueNotifier<double>>(
          param.key,
          ValueNotifier<double>(param.value.init),
        ),
      );

  /// Initializes the value notifiers for the checkboxes.
  /// Walks over all parameters and creates [MapEntry]s.
  /// The 'key' is the same as the key in their original [juceSliderParameters].
  /// The value is a [ValueNotifier] initialized with the corresponding default value of the parameter.
  Iterable<MapEntry<String, ValueNotifier<Object>>> initCheckboxNotifiers() =>
      juceCheckboxParameters.entries.map(
        (MapEntry<String, ({bool init, String title})> param) =>
            MapEntry<String, ValueNotifier<bool>>(
          param.key,
          ValueNotifier<bool>(param.value.init),
        ),
      );

  /// Builds a slider for a parameter that is coppled with a [ValueNotifier].
  /// The slider is configured as specified in the [juceSliderParameters].
  ValueListenableBuilder<double>? sliderBuilder(String key) {
    final ({
      double delta,
      double init,
      String Function(double p1) labelBuilder,
      double max,
      double min,
      String title,
      bool togglable
    })? parameter = juceSliderParameters[key];
    if (parameter == null) return null;
    return ValueListenableBuilder<double>(
      valueListenable: notifiers[key]! as ValueNotifier<double>,
      builder: (BuildContext context, double value, Widget? child) =>
          VoiceChangerSlider(
        title: parameter.title,
        value: value,
        min: parameter.min,
        max: parameter.max,
        delta: parameter.delta,
        onChanged: (double value) {
          debugPrint("${notifiers[key]}");
          debugPrint("$value");
          notifiers[key]?.value = value;
        },
        togglable: parameter.togglable,
        labelBuilder: parameter.labelBuilder,
      ),
    );
  }

  @override
  Future<void> dispose() async {
    // cleanup value notifiers
    for (final ValueNotifier<Object> notifier in notifiers.values) {
      notifier.dispose();
    }
    super.dispose();
  }

  @override
  Widget build(BuildContext context) => Scaffold(
        appBar: const ReVoiceMeAppBar(),
        drawer: const ReVoiceMeDrawer(
          cleanup: DeviceVolumeSlider.cleanup,
        ),
        body: Center(
          child: ListView(
            padding: const EdgeInsets.symmetric(
              vertical: CustomThemeData.defaultPadding,
            ),
            children: <Widget>[
              const DeviceVolumeSlider(),
              for (final String key in juceSliderParameters.keys)
                sliderBuilder(key) ?? const SizedBox.shrink(),
            ],
          ),
        ),
      );
}
