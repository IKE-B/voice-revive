import "package:flutter/material.dart";

/// A callback function that accepts a double value.
typedef DoubleCallback = void Function(double value);

/// A widget for manipulating the audio I/O via a user controllable slider.
///
/// The [AudioChanger] consists of a slider, a switch, buttons for increasing or decreasing the [value] and a descriptive [title].
/// It consumes a [value] from the parent widget and reflects it on the slider.
/// The [value] can be changed by the slider or the buttons.
/// The parent widget is notified via the [onChangedValue] function.
/// The [maxValue] controls the maximum logical value of the slider.
/// The switch's state is correlated with the [_AudioChangerState._isActive] variable.
/// It controls whether this widget interferes with the given [value] or not.
class AudioChanger extends StatefulWidget {
  /// Creates a new [AudioChanger] widget.
  ///
  /// All four parameters, [title], [value], [maxValue], and [onChangedValue], are required.
  const AudioChanger({
    required this.title,
    required this.value,
    required this.maxValue,
    required this.onChangedValue,
    super.key,
  });

  /// The maximum value of this widget. The minimum value is controlled by [_AudioChangerState._minValue].
  final double maxValue;

  /// The action that should be performed when the user changes the slider or presses the increase or decrease buttons.
  final DoubleCallback onChangedValue;

  /// The title of this widget. It is displayed above the slider.
  final String title;

  /// The current value. Provided by the parent widget.
  final double value;

  @override
  State<AudioChanger> createState() => _AudioChangerState();
}

class _AudioChangerState extends State<AudioChanger> {
  /// The number of steps of the slider. The device volume slider has 15 steps.
  static const int steps = 15;

  /// The minimum value of the slider.
  static const double _minValue = 0.0;

  /// Whether this widget currently interferes with the [AudioChanger.value].
  /// It is controlled by this widget's switch.
  bool _isActive = false;

  void _changeValue(double value) {
    widget.onChangedSlider(value);
    if (value < 0.0 || value > widget.maxValue) {
      return;
    }
  }

  void _setActive(bool value) {
    setState(() {
      _isActive = value;
    });
  }

  void _increaseValue() {
    if (widget.value < widget.maxValue) {
      _changeValue(widget.value + 0.1);
    }
  }

  void _decreaseValue() {
    if (widget.value > 0.0) {
      _changeValue(widget.value - 0.1);
    }
  }

  @override
  Widget build(BuildContext context) => Column(
        children: <Widget>[
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 50.0),
            child: Row(
              children: <Widget>[
                Text(
                  widget.title,
                  style: const TextStyle(
                    fontSize: 24,
                  ),
                ),
                Switch(
                  value: _isActive,
                  activeColor: Colors.green,
                  onChanged: _setActive,
                ),
              ],
            ),
          ),
          if (_isActive)
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: <Widget>[
                IconButton(
                  icon: const Icon(Icons.remove),
                  onPressed: _decreaseValue,
                ),
                Slider(
                  value: widget.value,
                  onChanged: _changeValue,
                  max: widget.maxValue,
                  divisions: 50,
                  label: widget.value.toStringAsFixed(1),
                ),
                IconButton(
                  icon: const Icon(Icons.add),
                  onPressed: _increaseValue,
                ),
              ],
            ),
        ],
      );
}
