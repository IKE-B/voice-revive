/// Static configuration of all JUCE parameters.
///
/// {@category Other}
final Map<String, Map<String, Record>> juceParameters =
    <String, Map<String, Record>>{
  "checkboxes": juceCheckboxParameters,
  "dropdowns": juceDropdownParameters,
  "sliders": juceSliderParameters,
};

/// Static configuration of all JUCE parameters that can be controlled with checkboxes.
///
/// {@category Other}
final Map<String, ({String title, bool init})> juceCheckboxParameters =
    <String, ({String title, bool init})>{
  "compAllBypassed": (title: "CompAll Bypassed", init: false),
  "compAllMute": (title: "CompAll Mute", init: false),
  "compLowBypassed": (title: "CompLow Bypassed", init: false),
  "compLowMute": (title: "CompLow Mute", init: false),
  "compLowSolo": (title: "CompLow Solo", init: false),
  "compMidBypassed": (title: "CompMid Bypassed", init: false),
  "compMidMute": (title: "CompMid Mute", init: false),
  "compMidSolo": (title: "CompMid Solo", init: false),
  "compHighBypassed": (title: "CompHigh Bypassed", init: false),
  "compHighMute": (title: "CompHigh Mute", init: false),
  "compHighSolo": (title: "CompHigh Solo", init: false),
};

/// Static configuration of all JUCE parameters that can be controlled with dropdown lists.
///
/// {@category Other}
final Map<String, ({String title, double init, List<double> values})>
    juceDropdownParameters =
    <String, ({String title, double init, List<double> values})>{
  "compAllRatio": (
    title: "CompAll Ratio",
    init: 3.0,
    values: <double>[
      1.0,
      1.5,
      2.0,
      3.0,
      4.0,
      5.0,
      6.0,
      7.0,
      8.0,
      10.0,
      15.0,
      20.0,
      50.0,
      100.0,
    ]
  ),
  "compLowRatio": (
    title: "CompLow Ratio",
    init: 3.0,
    values: <double>[
      1.0,
      1.5,
      2.0,
      3.0,
      4.0,
      5.0,
      6.0,
      7.0,
      8.0,
      10.0,
      15.0,
      20.0,
      50.0,
      100.0,
    ]
  ),
  "compMidRatio": (
    title: "CompMid Ratio",
    init: 3.0,
    values: <double>[
      1.0,
      1.5,
      2.0,
      3.0,
      4.0,
      5.0,
      6.0,
      7.0,
      8.0,
      10.0,
      15.0,
      20.0,
      50.0,
      100.0,
    ]
  ),
  "compHighRatio": (
    title: "CompHigh Ratio",
    init: 1,
    values: <double>[
      1.0,
      1.5,
      2.0,
      3.0,
      4.0,
      5.0,
      6.0,
      7.0,
      8.0,
      10.0,
      15.0,
      20.0,
      50.0,
      100.0,
    ]
  ),
  "lowCutSlope": (
    title: "LowCutSlope",
    init: 12,
    values: <double>[12, 24, 32, 48]
  ),
  "highCutSlope": (
    title: "HighCutSlope",
    init: 12,
    values: <double>[12, 24, 32, 48]
  ),
};

/// Static configuration of all JUCE parameters that can be controlled with sliders.
///
/// {@category Other}
final Map<
    String,
    ({
      String title,
      double min,
      double max,
      double delta,
      double init,
      String Function(double) labelBuilder,
      bool togglable
    })> juceSliderParameters = <String,
    ({
  String title,
  double min,
  double max,
  double delta,
  double init,
  String Function(double) labelBuilder,
  bool togglable
})>{
  "gain": (
    title: "Gain",
    min: -24,
    max: 24,
    delta: 0.5,
    init: 0,
    labelBuilder: (double value) => value.toStringAsFixed(1),
    togglable: false
  ),
  "compAllAttack": (
    title: "CompAll Attack",
    min: 5,
    max: 500,
    delta: 1,
    init: 50,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "compAllRelease": (
    title: "CompAll Release",
    min: 5,
    max: 500,
    delta: 1,
    init: 250,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "compAllThreshold": (
    title: "CompAll Threshold",
    min: -60,
    max: 12,
    delta: 1,
    init: 0,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "compLowAttack": (
    title: "CompLow Attack",
    min: 5,
    max: 500,
    delta: 1,
    init: 50,
    togglable: false,
    labelBuilder: (double value) => value.toStringAsFixed(0)
  ),
  "compLowRelease": (
    title: "CompLow Release",
    min: 5,
    max: 500,
    delta: 1,
    init: 250,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "compLowThreshold": (
    title: "CompLow Threshold",
    min: -60,
    max: 12,
    delta: 1,
    init: 0,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "compMidAttack": (
    title: "CompMid Attack",
    min: 5,
    max: 500,
    delta: 1,
    init: 50,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "compMidRelease": (
    title: "CompMid Release",
    min: 5,
    max: 500,
    delta: 1,
    init: 250,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "compMidThreshold": (
    title: "CompMid Threshold",
    min: -60,
    max: 12,
    delta: 1,
    init: 0,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "compHighAttack": (
    title: "CompHigh Attack",
    min: 5,
    max: 500,
    delta: 1,
    init: 50,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "compHighRelease": (
    title: "CompHigh Release",
    min: 5,
    max: 500,
    delta: 1,
    init: 250,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "compHighThreshold": (
    title: "CompHigh Threshold",
    min: -60,
    max: 12,
    delta: 1,
    init: 0,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "lowMidCutoff": (
    title: "LowMidCutoff",
    min: 20,
    max: 999,
    delta: 1,
    init: 400,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "midHighCutoff": (
    title: "MidHighCutoff",
    min: 1000,
    max: 20000,
    delta: 1,
    init: 2000,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "lowCutFreq": (
    title: "LowCutFreq",
    min: 20,
    max: 20000,
    delta: 1,
    init: 20,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "highCutFreq": (
    title: "HighCutFreq",
    min: 20,
    max: 20000,
    delta: 1,
    init: 20000,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "peakFreq": (
    title: "PeakFreq",
    min: 20,
    max: 20000,
    delta: 1,
    init: 750,
    labelBuilder: (double value) => value.toStringAsFixed(0),
    togglable: false
  ),
  "peakGainInDecibels": (
    title: "PeakGainInDecibels",
    min: -24,
    max: 24,
    delta: 1,
    init: 0,
    labelBuilder: (double value) => value.toStringAsFixed(1),
    togglable: false
  ),
  "peakQuality": (
    title: "PeakQuality",
    min: 0.1,
    max: 10,
    delta: 0.05,
    init: 1,
    labelBuilder: (double value) => value.toStringAsFixed(2),
    togglable: false
  ),
};
