import "dart:async";
import "package:flutter/material.dart";
import "package:revoiceme/core/utils/constants/juce_parameters.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";
import "package:revoiceme/core/utils/widgets/device_volume_slider/presentation/widget/device_volume_slider.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_appbar/presentation/widget/revoiceme_appbar.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_drawer/presentation/widget/revoiceme_drawer.dart";
import "package:revoiceme/core/utils/widgets/small_padder/presentation/widget/small_padder.dart";
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
    // reorder notifiers, for maps the order is the order of insertion
    // so to integrate the three original maps into each other and to reorder them as wanted
    // the entries have to be removed and reinserted in the wanted order
    notifiers.addEntries(<MapEntry<String, ValueNotifier<Object>>>[
      MapEntry<String, ValueNotifier<Object>>(
        "gain",
        notifiers.remove("gain")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compAllAttack",
        notifiers.remove("compAllAttack")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compAllRelease",
        notifiers.remove("compAllRelease")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compAllThreshold",
        notifiers.remove("compAllThreshold")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compAllRatio",
        notifiers.remove("compAllRatio")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compAllBypassed",
        notifiers.remove("compAllBypassed")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compAllMute",
        notifiers.remove("compAllMute")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compLowAttack",
        notifiers.remove("compLowAttack")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compLowRelease",
        notifiers.remove("compLowRelease")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compLowThreshold",
        notifiers.remove("compLowThreshold")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compLowRatio",
        notifiers.remove("compLowRatio")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compLowBypassed",
        notifiers.remove("compLowBypassed")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compLowMute",
        notifiers.remove("compLowMute")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compLowSolo",
        notifiers.remove("compLowSolo")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compMidAttack",
        notifiers.remove("compMidAttack")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compMidRelease",
        notifiers.remove("compMidRelease")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compMidThreshold",
        notifiers.remove("compMidThreshold")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compMidRatio",
        notifiers.remove("compMidRatio")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compMidBypassed",
        notifiers.remove("compMidBypassed")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compMidMute",
        notifiers.remove("compMidMute")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compMidSolo",
        notifiers.remove("compMidSolo")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compHighAttack",
        notifiers.remove("compHighAttack")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compHighRelease",
        notifiers.remove("compHighRelease")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compHighThreshold",
        notifiers.remove("compHighThreshold")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compHighRatio",
        notifiers.remove("compHighRatio")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compHighBypassed",
        notifiers.remove("compHighBypassed")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compHighMute",
        notifiers.remove("compHighMute")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "compHighSolo",
        notifiers.remove("compHighSolo")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "lowMidCutoff",
        notifiers.remove("lowMidCutoff")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "midHighCutoff",
        notifiers.remove("midHighCutoff")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "lowCutFreq",
        notifiers.remove("lowCutFreq")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "highCutFreq",
        notifiers.remove("highCutFreq")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "peakFreq",
        notifiers.remove("peakFreq")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "peakGainInDecibels",
        notifiers.remove("peakGainInDecibels")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "peakQuality",
        notifiers.remove("peakQuality")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "lowCutSlope",
        notifiers.remove("lowCutSlope")!,
      ),
      MapEntry<String, ValueNotifier<Object>>(
        "highCutSlope",
        notifiers.remove("highCutSlope")!,
      ),
    ]);
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

  /// Builds a dropdown for a parameter that is coppled with a [ValueNotifier].
  /// The dropdown is configured as specified in the [juceDropdownParameters].
  ValueListenableBuilder<double>? dropdownBuilder(String key) {
    final ({double init, String title, List<double> values})? parameter =
        juceDropdownParameters[key];
    if (parameter == null) return null;
    return ValueListenableBuilder<double>(
      valueListenable: notifiers[key]! as ValueNotifier<double>,
      builder: (BuildContext context, double value, Widget? child) => Container(
        padding: const EdgeInsets.symmetric(
          horizontal: CustomThemeData.defaultPadding,
        ),
        child: Row(
          children: <Widget>[
            Text(
              "${parameter.title}:",
              style: Theme.of(context).textTheme.bodyLarge,
            ),
            const SmallPadder(),
            DropdownButton<double>(
              value: value,
              items: parameter.values
                  .map(
                    (double value) => DropdownMenuItem<double>(
                      value: value,
                      child: Text(value.toStringAsFixed(1)),
                    ),
                  )
                  .toList(),
              onChanged: (double? value) {
                notifiers[key]?.value =
                    value ?? juceDropdownParameters[key]!.init;
              },
            ),
          ],
        ),
      ),
    );
  }

  /// Builds a checkbox for a parameter that is coppled with a [ValueNotifier].
  /// The checkbox is configured as specified in the [juceCheckboxParameters].
  ValueListenableBuilder<bool>? checkboxBuilder(String key) {
    final ({bool init, String title})? parameter = juceCheckboxParameters[key];
    if (parameter == null) return null;
    return ValueListenableBuilder<bool>(
      valueListenable: notifiers[key]! as ValueNotifier<bool>,
      builder: (BuildContext context, bool value, Widget? child) =>
          CheckboxListTile(
        title: Text(parameter.title),
        value: value,
        onChanged: (bool? value) {
          notifiers[key]?.value = value ?? juceCheckboxParameters[key]!.init;
        },
      ),
    );
  }

  /// Selects the right builder for a parameter.
  ValueListenableBuilder<Object>? selectBuilder(String key) {
    if (juceSliderParameters.containsKey(key)) {
      return sliderBuilder(key);
    } else if (juceDropdownParameters.containsKey(key)) {
      return dropdownBuilder(key);
    } else if (juceCheckboxParameters.containsKey(key)) {
      return checkboxBuilder(key);
    }
    return null;
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
              for (final String key in notifiers.keys) ...<Widget>[
                selectBuilder(key) ?? const SizedBox.shrink(),
                const SmallPadder(),
              ],
            ],
          ),
        ),
      );
}
