import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_sound/flutter_sound.dart';
import 'package:permission_handler/permission_handler.dart';
import 'package:flutter/foundation.dart';
import 'package:volume_controller/volume_controller.dart';
import 'audio_changer.dart';

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
  late AudioChanger audioVolume;
  late AudioChanger deviceVolume;

  @override
  void initState() {
    super.initState();
    audioVolume = AudioChanger(
      onChangedSlider: (double value) {},
      title: 'Audio-Lautstärke: ',
      maxValue: 5.0,
    );
    deviceVolume = AudioChanger(
      onChangedSlider: (double volume) {
        if (volume >= 1.0) {
          _showWarningMaxVolume();
        } else if (volume >= 0.0) {
          VolumeController().setVolume(volume);
        }
      },
      title: 'Geräte-Lautstärke: ',
      maxValue: 1.0,
    );
    VolumeController().listener((volume) {
      if (kDebugMode) {
        print('Volume changed: $volume');
      }
      setState(() {
        deviceVolume.value = volume;
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
    await _player.setVolume(1.0);
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
      deviceVolume.value = volume;
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
              deviceVolume,
              audioVolume,
            ]),
      ),
    );
  }
}
