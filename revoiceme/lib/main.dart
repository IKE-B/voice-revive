import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_sound/flutter_sound.dart';
import 'package:permission_handler/permission_handler.dart';
import 'package:flutter/foundation.dart';
import 'package:volume_controller/volume_controller.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
        useMaterial3: true,
      ),
      home: const VoiceAmplifier(title: 'Flutter Demo Home Page'),
    );
  }
}

class VoiceAmplifier extends StatefulWidget {
  const VoiceAmplifier({super.key, required this.title});

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  _VoiceAmplifierState createState() => _VoiceAmplifierState();
}

class _VoiceAmplifierState extends State<VoiceAmplifier> {
  final FlutterSoundPlayer _player = FlutterSoundPlayer();
  final FlutterSoundRecorder _recorder = FlutterSoundRecorder();
  bool _isAmplifying = false;
  bool _isDeviceVolumeActive = false;
  bool _isAudioVolumeActive = false;
  double _currentDeviceVolume = 0.5;
  double _currentAudioVolume = 1.0;
  double _maxAudioVolume = 5.0;

  @override
  void initState() {
    super.initState();
    VolumeController().listener((volume) {
      if (kDebugMode) {
        print('Volume changed: $volume');
      }
      setState(() {
        _currentDeviceVolume = volume;
      });
    });
    VolumeController().showSystemUI = true;
    _initVolume();
    _initializeAudioSessions();
  }

  Future<void> _initializeAudioSessions() async {
    // Request microphone permission
    var status = await Permission.microphone.request();
    if (status != PermissionStatus.granted) {
      throw RecordingPermissionException("Microphone permission not granted");
    }

    await _player.openPlayer();
    await _recorder.openRecorder();
  }

  @override
  void dispose() {
    _player.closePlayer();
    _recorder.closeRecorder();
    VolumeController().removeListener();
    super.dispose();
  }

  Future<void> _initVolume() async {
    double volume = await VolumeController().getVolume();
    setState(() {
      _currentDeviceVolume = volume;
    });
  }

  Future<void> _showWarningMaxVolume() async {
    showDialog<void>(
      context: context,
      barrierDismissible: true, // user must tap button!
      builder: (BuildContext context) {
        return AlertDialog(
          title: const Text('Maximale Lautstärke erreicht'),
          content: const Text(
              'Stellen Sie sicher, dass der Lautsprecher auf voller Laustärke eingestellt ist.'),
          actions: <Widget>[
            TextButton(
              child: const Text('Ok'),
              onPressed: () {
                Navigator.of(context).pop();
              },
            ),
          ],
        );
      },
    );
  }

  void _toggleSlider() {
    setState(() {
      _isDeviceVolumeActive = !_isDeviceVolumeActive;
    });
  }

  void _changeVolumeDevice(double volume) async {
    if (volume < 0.0) {
      return;
    } else if (volume > 1.0) {
      await _showWarningMaxVolume();
      return;
    }
    setState(() {
      _currentDeviceVolume = volume;
    });
    VolumeController().setVolume(volume);
  }

  void _increaseVolumeDevice() {
    if (_currentDeviceVolume < 1.0) {
      _changeVolumeDevice((_currentDeviceVolume + 0.1));
    }
  }

  void _decreaseVolumeDevice() {
    if (_currentDeviceVolume > 0.0) {
      _changeVolumeDevice((_currentDeviceVolume - 0.1));
    }
  }

  void _changeVolumeAudio(double volume) async {
    if (volume < 0.0 || volume > _maxAudioVolume) {
      return;
    }
    setState(() {
      _currentAudioVolume = volume;
    });
  }

  void _increaseVolumeAudio() {
    if (_currentAudioVolume < _maxAudioVolume) {
      _changeVolumeAudio((_currentAudioVolume + 0.1));
    }
  }

  void _decreaseVolumeAudio() {
    if (_currentAudioVolume > 0.0) {
      _changeVolumeAudio((_currentAudioVolume - 0.1));
    }
  }

  Future<void> _startAmplifying() async {
    if (_isAmplifying) return;

    final _streamController = StreamController<Food>.broadcast();

    _streamController.stream.listen((food) {
      if (_player.isPlaying) {
        _player.foodSink!.add(food);
      }
    });

    await _player.startPlayerFromStream(
      codec: Codec.pcm16,
      numChannels: 1,
      sampleRate: 44100,
    );

    await _recorder.startRecorder(
      toStream: _streamController,
      codec: Codec.pcm16,
      sampleRate: 44100,
      numChannels: 1,
    );

    setState(() {
      _isAmplifying = true;
    });
  }

  Future<void> _stopAmplifying() async {
    if (!_isAmplifying) return;

    await _recorder.stopRecorder();
    await _player.stopPlayer();

    setState(() {
      _isAmplifying = false;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('ReVoiceMe')),
      body: Center(
        // Center is a layout widget. It takes a single child and positions it
        // in the middle of the parent.
        child: Column(
            // Column is also a layout widget. It takes a list of children and
            // arranges them vertically. By default, it sizes itself to fit its
            // children horizontally, and tries to be as tall as its parent.
            //
            // Column has various properties to control how it sizes itself and
            // how it positions its children. Here we use mainAxisAlignment to
            // center the children vertically; the main axis here is the vertical
            // axis because Columns are vertical (the cross axis would be
            // horizontal).
            //
            // TRY THIS: Invoke "debug painting" (choose the "Toggle Debug Paint"
            // action in the IDE, or press "p" in the console), to see the
            // wireframe for each widget.
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              Padding(
                padding: const EdgeInsets.symmetric(vertical: 25.0),
                child: Column(
                  children: [
                    const Text(
                      'Starte ReVoiceMe:',
                      style: TextStyle(
                        fontSize: 24,
                      ),
                    ),
                    IconButton(
                      onPressed:
                          _isAmplifying ? _stopAmplifying : _startAmplifying,
                      tooltip: 'Beginne deine Stimmveränderung',
                      icon: _isAmplifying
                          ? Icon(Icons.pause_circle)
                          : Icon(Icons.not_started),
                      color: _isAmplifying
                          ? Theme.of(context).iconTheme.color
                          : Colors.green,
                      iconSize: 72,
                    ), // This trailing comma makes auto-formatting nicer for build methods.
                    Text(
                      _isAmplifying
                          ? 'Stimmveränderung wird durchgeführt.'
                          : 'Stimmveränderung nicht aktiviert.',
                      style: Theme.of(context).textTheme.bodyLarge,
                    ),
                  ],
                ),
              ),
              Padding(
                padding: const EdgeInsets.symmetric(horizontal: 50.0),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.start,
                  children: [
                    const Text(
                      'Geräte-Lautstärke ',
                      style: TextStyle(
                        fontSize: 24,
                      ),
                    ),
                    Switch(
                      value: _isDeviceVolumeActive,
                      activeColor: Colors.green,
                      onChanged: (bool value) {
                        // This is called when the user toggles the switch.
                        setState(() {
                          _isDeviceVolumeActive = value;
                        });
                      },
                    ),
                  ],
                ),
              ),
              if (_isDeviceVolumeActive)
                Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    IconButton(
                      icon: Icon(Icons.remove),
                      onPressed: _decreaseVolumeDevice,
                    ),
                    Slider(
                      value: _currentDeviceVolume,
                      onChanged: _changeVolumeDevice,
                      min: 0.0,
                      max: 1.0,
                      divisions: 50,
                      label: _currentDeviceVolume.round().toString(),
                    ),
                    IconButton(
                      icon: Icon(Icons.add),
                      onPressed: _increaseVolumeDevice,
                    ),
                  ],
                ),
              Padding(
                padding: const EdgeInsets.symmetric(horizontal: 50.0),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.start,
                  children: [
                    const Text(
                      'Audio-Lautstärke: ',
                      style: TextStyle(
                        fontSize: 24,
                      ),
                    ),
                    Switch(
                      value: _isAudioVolumeActive,
                      activeColor: Colors.green,
                      onChanged: (bool value) {
                        // This is called when the user toggles the switch.
                        setState(() {
                          _isAudioVolumeActive = value;
                        });
                      },
                    ),
                  ],
                ),
              ),
              if (_isAudioVolumeActive)
                Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    IconButton(
                      icon: Icon(Icons.remove),
                      onPressed: _decreaseVolumeAudio,
                    ),
                    Slider(
                      value: _currentAudioVolume,
                      onChanged: _changeVolumeAudio,
                      min: 0.0,
                      max: _maxAudioVolume,
                      divisions: 50,
                      label: _currentAudioVolume.toStringAsFixed(1),
                    ),
                    IconButton(
                      icon: Icon(Icons.add),
                      onPressed: _increaseVolumeAudio,
                    ),
                  ],
                ),
            ]),
      ),
    );
  }
}
