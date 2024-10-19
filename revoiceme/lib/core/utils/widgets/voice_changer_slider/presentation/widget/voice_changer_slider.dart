import "package:flutter/material.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";

/// A callback function that accepts a double value.
typedef DoubleCallback = void Function(double value);

/// A widget for controlling a certain voice changing parameter via a user controllable slider.
///
/// The [VoiceChangerSlider] consists of a a descriptive [title] and
/// a slider for increasing or decreasing the [value].
/// Optionally, the slider can be activated or deactivated by a switch.
/// The switch's state is correlated with the [_VoiceChangerSliderState._isActive] variable.
///
/// {@category Widget}
class VoiceChangerSlider extends StatefulWidget {
  /// Creates a new [VoiceChangerSlider] widget.
  const VoiceChangerSlider({
    required this.title,
    required this.value,
    required this.min,
    required this.max,
    required this.delta,
    required this.onChanged,
    this.togglable = false,
    this.labelBuilder = _defaultLabelBuilder,
    super.key,
  });

  /// The title of this widget. It is displayed above the slider.
  final String title;

  /// The current value.
  final double value;

  /// The minimum value of the slider (left end).
  final double min;

  /// The maximum value of the slider (right end).
  final double max;

  /// The step size of the slider.
  final double delta;

  /// Whether the slider can be toggled on or off by a switch ('false' by default).
  final bool togglable;

  /// The action that should be performed when the user changes the slider.
  final DoubleCallback onChanged;

  /// A function that builds the display text above the handle of the slider.
  final String Function(double value) labelBuilder;

  /// The default label builder for the slider. Displays the percentage of the value between the min and max.
  static String _defaultLabelBuilder(double value) => value.toStringAsFixed(0);

  @override
  State<VoiceChangerSlider> createState() => _VoiceChangerSliderState();
}

class _VoiceChangerSliderState extends State<VoiceChangerSlider> {
  /// Whether this widget currently interferes with the [VoiceChangerSlider.value].
  /// It is controlled by this widget's switch.pop
  bool _isActive = false;

  @override
  void initState() {
    super.initState();
    _isActive = !widget.togglable;
  }

  /// A function for limiting valid scenarios for calling [VoiceChangerSlider.onChanged].
  void _onChanged(double value) {
    // don't call [VoiceChangerSlider.onChanged] if the widget is inactive
    if (!_isActive) {
      return;
    }

    // only call [VoiceChangerSlider.onChanged] with the new [VoiceChangerSlider.value] if it is within the valid range,
    // otherwise call it with the boundary values [VoiceChangerSlider.min] or [VoiceChangerSlider.max]
    if (widget.value < widget.min) {
      widget.onChanged(widget.min);
    } else if (widget.value > widget.max) {
      widget.onChanged(widget.max);
    } else {
      widget.onChanged(value);
    }
  }

  void _setActive(bool value) {
    setState(() {
      _isActive = value;
    });
  }

  @override
  Widget build(BuildContext context) => Padding(
        padding: const EdgeInsets.symmetric(
          horizontal: CustomThemeData.defaultPadding,
        ),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: <Widget>[
            Text(
              "${widget.title}: ${widget.labelBuilder(widget.value)}",
              style: Theme.of(context).textTheme.bodyLarge,
            ),
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: <Widget>[
                if (widget.togglable)
                  Padding(
                    padding: const EdgeInsets.only(
                      left: CustomThemeData.smallPadding,
                    ),
                    child: Switch(
                      value: _isActive,
                      onChanged: _setActive,
                    ),
                  ),
                Expanded(
                  child: Slider(
                    value: widget.value,
                    onChanged: _isActive ? _onChanged : null,
                    min: widget.min,
                    max: widget.max,
                    divisions:
                        ((widget.max - widget.min) / widget.delta).toInt(),
                  ),
                ),
              ],
            ),
          ],
        ),
      );
}
