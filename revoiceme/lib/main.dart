
import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_sound/flutter_sound.dart';
import 'package:permission_handler/permission_handler.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: Text('Real-time Voice Amplifier'),
        ),
        body: VoiceAmplifier(),
      ),
    );
  }
}

class VoiceAmplifier extends StatefulWidget {
  @override
  _VoiceAmplifierState createState() => _VoiceAmplifierState();
}

class _VoiceAmplifierState extends State<VoiceAmplifier> {
  final FlutterSoundPlayer _player = FlutterSoundPlayer();
  final FlutterSoundRecorder _recorder = FlutterSoundRecorder();
  bool _isRecording = false;

  @override
  void initState() {
    super.initState();
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
    super.dispose();
  }

  Future<void> _startAmplifying() async {
    if (_isRecording) return;

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
      _isRecording = true;
    });
  }

  Future<void> _stopAmplifying() async {
    if (!_isRecording) return;

    await _recorder.stopRecorder();
    await _player.stopPlayer();

    setState(() {
      _isRecording = false;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: <Widget>[
          ElevatedButton(
            onPressed: _isRecording ? _stopAmplifying : _startAmplifying,
            child: Text(_isRecording ? 'Stop Amplifying' : 'Start Amplifying'),
          ),
        ],
      ),
    );
  }
}
