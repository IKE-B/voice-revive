import "dart:async";

import "package:flutter_bloc/flutter_bloc.dart";
import "package:flutter_sound/flutter_sound.dart";
import "package:permission_handler/permission_handler.dart";
import "package:revoiceme/core/utils/widgets/voice_change_demo/presentation/widget/voice_change_demo_event.dart";
import "package:revoiceme/core/utils/widgets/voice_change_demo/presentation/widget/voice_change_demo_state.dart";

// TODO(L483): Integrate the app with the real voice changing feature.

/// A concept demonstration for the voice changing feature.
///
/// After the [VoiceChangeDemo] is started,
/// it records the microphone input and relays it to the device's speakers instantly.
/// It is organized as a [Bloc].
///
/// {@category Global}
/// {@category Other}
class VoiceChangeDemo extends Bloc<VoiceChangeDemoEvent, VoiceChangeDemoState> {
  /// Creates a new [VoiceChangeDemo] instance.
  /// Couples together [VoiceChangeDemoEvent] and [VoiceChangeDemoState].
  /// Defines the logic to execute between receiving events and emitting states.
  VoiceChangeDemo() : super(const VoiceChangeDemoState()) {
    // attempt to initialize the [_recorder] and [_player] sessions, then emit the 'initialized' state
    on<Init>((Init event, Emitter<VoiceChangeDemoState> emit) async {
      // request permission to use the microphone

      if (await Permission.microphone.request() != PermissionStatus.granted) {
        return;
      }
      await _initializeAudioSessions();
      emit(
        VoiceChangeDemoState(
          isInitialized: true,
          isChangingVoice: state.isChangingVoice,
          isToggleAllowed: state.isToggleAllowed,
        ),
      );
    });

    // dispose the [_recorder] and [_player] sessions, then emit the 'disposed' state
    on<Dispose>((Dispose event, Emitter<VoiceChangeDemoState> emit) async {
      await _dispose();
      emit(
        VoiceChangeDemoState(isToggleAllowed: state.isToggleAllowed),
      );
    });

    // attempt to start changing, then emit the 'started changing' state
    on<StartChanging>(
        (StartChanging event, Emitter<VoiceChangeDemoState> emit) async {
      // don't start changing if the [_recorder] or [_player] are not initialized
      // don't start changing if toggling is not allowed
      if (!state.isInitialized || !state.isToggleAllowed) return;
      await _startChanging();
      emit(
        VoiceChangeDemoState(
          isInitialized: state.isInitialized,
          isChangingVoice: true,
          isToggleAllowed: state.isToggleAllowed,
        ),
      );
    });

    // attempt to stop changing, then emit the 'stopped changing' state
    on<StopChanging>(
        (StopChanging event, Emitter<VoiceChangeDemoState> emit) async {
      // don't stop changing if toggling is not allowed
      if (!state.isToggleAllowed) return;
      await _stopChanging();
      emit(
        VoiceChangeDemoState(
          isInitialized: state.isInitialized,
          isToggleAllowed: state.isToggleAllowed,
        ),
      );
    });

    // emit the 'allow toggle' state
    on<AllowToggle>((AllowToggle event, Emitter<VoiceChangeDemoState> emit) {
      emit(
        VoiceChangeDemoState(
          isInitialized: state.isInitialized,
          isChangingVoice: state.isChangingVoice,
        ),
      );
    });

    // emit the 'disallow toggle' state
    on<DisallowToggle>(
        (DisallowToggle event, Emitter<VoiceChangeDemoState> emit) {
      emit(
        VoiceChangeDemoState(
          isInitialized: state.isInitialized,
          isChangingVoice: state.isChangingVoice,
          isToggleAllowed: false,
        ),
      );
    });
  }

  /// Records a voice stream from the device's microphone.
  final FlutterSoundRecorder _recorder = FlutterSoundRecorder();

  /// Plays the recorded voice stream via the device's speakers.
  final FlutterSoundPlayer _player = FlutterSoundPlayer();

  /// A stream controller for the voice stream for adding recorded voice data
  /// from the [_recorder] to the [_player] stream.
  final StreamController<Food> _streamController =
      StreamController<Food>.broadcast();

  /// The maximum volume of the device.
  static const double maxVolume = 1.0;

  /// Initializes the [_recorder] and [_player] sessions.
  Future<void> _initializeAudioSessions() async {
    // init [_player]
    await _player.openPlayer();
    await _player.setVolume(maxVolume / 2);

    // init [_recorder]
    await _recorder.openRecorder();
  }

  /// Disposes the audio sessions for the [_recorder] and [_player].
  /// Closes the [_streamController] stream.
  Future<void> _dispose() async {
    await _stopChanging();
    await _player.closePlayer();
    await _recorder.closeRecorder();
    await _streamController.close();
  }

  /// Starts or resumes the voice changing process.
  Future<void> _startChanging() async {
    // don't attempt to start changing if the changing is already in progress
    if (state.isChangingVoice) return;
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
  }

  /// Stops the voice changing process without disposing the [_recorder] and [_player] sessions.
  Future<void> _stopChanging() async {
    await _recorder.stopRecorder();
    await _player.stopPlayer();
  }
}
