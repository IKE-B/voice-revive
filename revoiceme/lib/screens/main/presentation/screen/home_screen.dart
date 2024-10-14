import "dart:async";
import "package:flutter/material.dart";
import "package:flutter_sound/flutter_sound.dart";
import "package:permission_handler/permission_handler.dart";
import "package:revoiceme/core/routers/router.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_appbar/presentation/widget/revoiceme_appbar.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_drawer/presentation/widget/revoiceme_drawer.dart";
import "package:revoiceme/core/utils/widgets/small_padder/presentation/widget/small_padder.dart";
import "package:revoiceme/core/utils/widgets/voice_changer_control/presentation/widget/voice_changer_control.dart";
import "package:revoiceme/core/utils/widgets/voice_changer_slider/presentation/widget/voice_changer_slider.dart";
import "package:volume_controller/volume_controller.dart";

/// The home screen of the ReVoiceMe app.
///
/// {@category Screen}
class HomeScreen extends StatefulWidget {
  /// Creates a new [HomeScreen] instance.
  const HomeScreen({
    super.key,
  });

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  /// Listens to changes in the device volume. Syncs system controls with in app volume controls.
  ValueNotifier<double> deviceVolume = ValueNotifier<double>(0.0);

  /// Listens to changes in the pitch parameter for the voice changer. Only a stub for now.
  ValueNotifier<double> pitch = ValueNotifier<double>(1.5);

  /// Records a voice stream from the device's microphone.
  final FlutterSoundRecorder _recorder = FlutterSoundRecorder();

  /// Plays the recorded and altered voice stream via the device's speakers.
  final FlutterSoundPlayer _player = FlutterSoundPlayer();

  /// Whether the [_recorder] has been initialized and is ready to record.
  bool _isRecorderInitialized = false;

  /// Whether the [_player] has been initialized and is ready to play.
  bool _isPlayerInitialized = false;

  /// Whether the voice changer is currently amplifying the voice.
  bool _isAmplifying = false;

  /// A stream controller for the voice stream for adding recorded voice data from the [_recorder] to the [_player] stream.
  final StreamController<Food> _streamController =
      StreamController<Food>.broadcast(
    onCancel: () => debugPrint("Stream closed"),
    onListen: () => debugPrint("Stream opened"),
  );

  static const double minVolume = 0.0;
  static const double maxVolume = 1.0;

  @override
  Future<void> dispose() async {
    // handle [_player] cleanup
    await _player.stopPlayer();
    await _player.closePlayer();

    // handle [_recorder] cleanup
    await _recorder.stopRecorder();
    await _recorder.closeRecorder();

    // handle [_streamController] cleanup
    await _streamController.close();

    // cleanup value notifiers
    VolumeController().removeListener();
    deviceVolume.dispose();
    pitch.dispose();

    super.dispose();
  }

  @override
  void initState() {
    super.initState();
    VolumeController().listener((double volume) {
      debugPrint("Volume changed: $volume");
      deviceVolume.value = volume;
    });
    // don't show because its popin animation plays after every value change
    VolumeController().showSystemUI = false;
    unawaited(_initVolume());
    unawaited(_initializeAudioSessions());
  }

  /// Initializes the audio sessions for the [_recorder] and [_player].
  Future<void> _initializeAudioSessions() async {
    // request permission to use the microphone
    final PermissionStatus status = await Permission.microphone.request();
    if (status != PermissionStatus.granted) {
      throw RecordingPermissionException("Mikrofonzugriff verweigert.");
    }

    // init [_player]
    await _player.openPlayer();
    await _player.setVolume(maxVolume / 2);
    setState(() {
      _isPlayerInitialized = true;
    });

    // init [_recorder]
    await _recorder.openRecorder();
    setState(() {
      _isRecorderInitialized = true;
    });
  }

  /// Get the initial volume of the device for the [deviceVolume] value notifier.
  Future<void> _initVolume() async {
    deviceVolume.value = await VolumeController().getVolume();
  }

  /// Shows a warning dialog and a helping message when the user tries to set the volume to a value above the maximum.
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

  /// Starts or resumes the voice amplification process.
  Future<void> _startAmplifying() async {
    // don't attempt to start amplifying if the amplification is already in progress
    if (_isAmplifying) return;
    // don't start amplifying if the [_recorder] is already recording or the [_player] is playing
    if (_recorder.isRecording || _player.isPlaying) return;
    // don't start amplifying if the [_recorder] or [_player] are not initialized
    if (!_isPlayerInitialized || !_isRecorderInitialized) return;

    // starts or resumes listening to the microphone input stream of [_recorder] and adds its data to the [_player] stream
    _streamController.stream.listen((Food food) {
      if (_player.isPlaying) {
        _player.foodSink!.add(food);
      }
    });

    // starts or resumes the playing of the voice stream via the device's speakers
    await _player.startPlayerFromStream(
      sampleRate: 44100,
      whenFinished: _streamController.close,
    );

    // starts or resumes the recording of the voice stream from the device's microphone
    await _recorder.startRecorder(
      toStream: _streamController,
      codec: Codec.pcm16,
      sampleRate: 44100,
    );

    // amplification process started successfully
    setState(() {
      _isAmplifying = true;
    });
  }

  /// Stops the voice amplification process.
  Future<void> _stopAmplifying() async {
    // don't attempt to stop amplifying if the amplification is not in progress
    if (!_isAmplifying) return;

    // stops the recording and playing of the voice stream
    await _recorder.stopRecorder();
    await _player.stopPlayer();

    // amplification process stopped successfully
    setState(() {
      _isAmplifying = false;
    });
  }

  /// Handles changes in the device volume and keeps the volume inside the device bounds.
  Future<void> _deviceVolumeOnChanged(double volume) async {
    if (volume >= maxVolume) {
      await _showWarningMaxVolume();
    } else if (volume >= minVolume) {
      VolumeController().setVolume(volume);
    }
  }

  @override
  Widget build(BuildContext context) => Scaffold(
        appBar: ReVoiceMeAppBar(
          title: Routes.homeText,
          userHasControl: true,
          appliesVoiceChanging: _isAmplifying,
          toggleVoiceChanging:
              _isAmplifying ? _stopAmplifying : _startAmplifying,
        ),
        drawer: const ReVoiceMeDrawer(Routes.home),
        body: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              VoiceChangerControl(
                toggleVoiceChanging:
                    _isAmplifying ? _stopAmplifying : _startAmplifying,
                userHasControl: true,
                appliesVoiceChanging: _isAmplifying,
                size: 80,
                activeIcon: Icons.pause_rounded,
                inactiveIcon: Icons.play_arrow_rounded,
              ),
              const SmallPadder(),
              ValueListenableBuilder<double>(
                valueListenable: deviceVolume,
                builder: (BuildContext context, double value, Widget? child) =>
                    VoiceChangerSlider(
                  title: "Lautstärke",
                  value: value,
                  min: minVolume,
                  max: maxVolume,
                  delta: maxVolume /
                      15, // 15 steps is the default step count for the (development) device's volume slider
                  togglable: false,
                  onChanged: _deviceVolumeOnChanged,
                ),
              ),
              ValueListenableBuilder<double>(
                valueListenable: pitch,
                builder: (BuildContext context, double value, Widget? child) =>
                    VoiceChangerSlider(
                  title: "Tonhöhe",
                  value: value,
                  min: -50,
                  max: 50,
                  delta: 5.0,
                  togglable: true,
                  onChanged: (double value) {
                    debugPrint("$pitch");
                    debugPrint("$value");
                    pitch.value = value;
                  },
                ),
              ),
            ],
          ),
        ),
      );
}
