import "dart:async";

import "package:flutter/material.dart";
import "package:flutter_sound/flutter_sound.dart";
import "package:permission_handler/permission_handler.dart";
import "package:revoiceme/audio_changer.dart";
import "package:volume_controller/volume_controller.dart";

void main() {
  runApp(const ReVoiceMe());
}

/// The main widget of the ReVoiceMe app.
class ReVoiceMe extends StatelessWidget {
  /// Creates a new [ReVoiceMe] instance.
  const ReVoiceMe({super.key});

  @override
  Widget build(BuildContext context) => MaterialApp(
        title: "ReVoiceMe",
        theme: ThemeData(
          colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
          useMaterial3: true,
        ),
        home: const VoiceAmplifier(),
      );
}

/// The home screen of the ReVoiceMe app.
class VoiceAmplifier extends StatefulWidget {
  /// Creates a new [VoiceAmplifier] instance.
  const VoiceAmplifier({
    super.key,
  });

  @override
  State<VoiceAmplifier> createState() => _VoiceAmplifierState();
}

class _VoiceAmplifierState extends State<VoiceAmplifier> {
  ValueNotifier<double> deviceVolume = ValueNotifier<double>(0.0);
  ValueNotifier<double> pitch = ValueNotifier<double>(1.5);

  bool _isAmplifying = false;
  final FlutterSoundPlayer _player = FlutterSoundPlayer();
  final FlutterSoundRecorder _recorder = FlutterSoundRecorder();
  final StreamController<Food> _streamController =
      StreamController<Food>.broadcast(
    onCancel: () => debugPrint("Stream closed"),
    onListen: () => debugPrint("Stream opened"),
  );

  @override
  Future<void> dispose() async {
    await _player.closePlayer();
    await _recorder.closeRecorder();
    await _streamController.close();
    VolumeController().removeListener();
    super.dispose();
  }

  @override
  void initState() {
    super.initState();
    VolumeController().listener((double volume) {
      debugPrint("Volume changed: $volume");
      deviceVolume.value = volume;
    });
    VolumeController().showSystemUI = true;
    unawaited(_initVolume());
    unawaited(_initializeAudioSessions());
  }

  Future<void> _initializeAudioSessions() async {
    // Request microphone permission
    final PermissionStatus status = await Permission.microphone.request();
    if (status != PermissionStatus.granted) {
      throw RecordingPermissionException("Mikrofonzugriff verweigert.");
    }

    await _player.openPlayer();
    await _recorder.openRecorder();
    await _player.setVolume(1.0);
  }

  Future<void> _initVolume() async {
    deviceVolume.value = await VolumeController().getVolume();
  }

  Future<void> _showWarningMaxVolume() async {
    await showDialog<void>(
      context: context,
      builder: (BuildContext context) => AlertDialog(
        title: const Text("Maximale Lautstärke erreicht"),
        content: const Text(
          "Stellen Sie sicher, dass der Lautsprecher auf voller Laustärke eingestellt ist.",
        ),
        actions: <Widget>[
          TextButton(
            child: const Text("Ok"),
            onPressed: () {
              Navigator.of(context).pop();
            },
          ),
        ],
      ),
    );
  }

  Future<void> _startAmplifying() async {
    if (_isAmplifying) return;

    _streamController.stream.listen((Food food) {
      if (_player.isPlaying) {
        _player.foodSink!.add(food);
      }
    });

    await _player.startPlayerFromStream(
      sampleRate: 44100,
    );

    await _recorder.startRecorder(
      toStream: _streamController,
      codec: Codec.pcm16,
      sampleRate: 44100,
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

  Future<void> _deviceVolumeOnChanged(double volume) async {
    if (volume >= 1.0) {
      await _showWarningMaxVolume();
    } else if (volume >= 0.0) {
      VolumeController().setVolume(volume);
    }
  }

  @override
  Widget build(BuildContext context) => Scaffold(
        appBar: AppBar(title: const Text("ReVoiceMe")),
        body: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              Padding(
                padding: const EdgeInsets.symmetric(vertical: 25.0),
                child: Column(
                  children: <Widget>[
                    const Text(
                      "Starte ReVoiceMe:",
                      style: TextStyle(
                        fontSize: 24,
                      ),
                    ),
                    IconButton(
                      onPressed:
                          _isAmplifying ? _stopAmplifying : _startAmplifying,
                      tooltip: "Starte deine Stimmveränderung.",
                      icon: _isAmplifying
                          ? const Icon(Icons.pause_circle)
                          : const Icon(Icons.not_started),
                      color: _isAmplifying
                          ? Theme.of(context).iconTheme.color
                          : Colors.green,
                      iconSize: 72,
                    ),
                    Text(
                      "Stimmveränderung ${_isAmplifying ? "aktiviert" : "deaktiviert"}.",
                      style: Theme.of(context).textTheme.bodyLarge,
                    ),
                  ],
                ),
              ),
              ValueListenableBuilder<double>(
                valueListenable: deviceVolume,
                builder: (BuildContext context, double value, Widget? child) =>
                    AudioChanger(
                  onChangedValue: _deviceVolumeOnChanged,
                  title: "Lautstärke:",
                  maxValue: 1.0,
                  value: value,
                ),
              ),
              ValueListenableBuilder<double>(
                valueListenable: pitch,
                builder: (BuildContext context, double value, Widget? child) =>
                    AudioChanger(
                  onChangedValue: (double value) {
                    debugPrint("$pitch");
                    debugPrint("$value");
                    pitch.value = value;
                  },
                  title: "Tonhöhe:",
                  maxValue: 3.0,
                  value: value,
                ),
              ),
            ],
          ),
        ),
      );
}
