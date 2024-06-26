import 'package:flutter/material.dart';

typedef DoubleCallback = void Function(double value);

// ignore: must_be_immutable
class AudioChanger extends StatefulWidget {
  /// AudioChanger is a widget that always implements a Slider to set a value and
  /// a button to activate or deactivate the change. The method onChangedSlider
  /// is always called when the value of the slider is changed.
  bool _isActive = false;
  double value = 1.0;
  double maxValue;
  final String title;
  final DoubleCallback onChangedSlider;

  AudioChanger({
    Key? key,
    required this.onChangedSlider,
    required this.title,
    required this.maxValue,
  }) : super(key: key);

  @override
  State<StatefulWidget> createState() => _AudioChangerState();
}

class _AudioChangerState extends State<AudioChanger> {
  void _changeValue(double value) {
    widget.onChangedSlider(value);
    if (value < 0.0 || value > widget.maxValue) {
      return;
    }
    setState(() {
      widget.value = value;
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
  Widget build(BuildContext context) {
    return Column(
      children: [
        Padding(
          padding: const EdgeInsets.symmetric(horizontal: 50.0),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.start,
            children: [
              Text(
                widget.title,
                style: const TextStyle(
                  fontSize: 24,
                ),
              ),
              Switch(
                value: widget._isActive,
                activeColor: Colors.green,
                onChanged: (bool value) {
                  // This is called when the user toggles the switch.
                  setState(() {
                    widget._isActive = value;
                  });
                },
              ),
            ],
          ),
        ),
        if (widget._isActive)
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              IconButton(
                icon: const Icon(Icons.remove),
                onPressed: _decreaseValue,
              ),
              Slider(
                value: widget.value,
                onChanged: _changeValue,
                min: 0.0,
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
}
