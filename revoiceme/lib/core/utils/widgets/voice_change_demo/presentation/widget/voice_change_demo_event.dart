/// The (abstract) class for all events in [VoiceChangeDemo].
///
/// {@category Other}
abstract class VoiceChangeDemoEvent {
  /// Creates a new instance of [VoiceChangeDemoEvent].
  const VoiceChangeDemoEvent();
}

/// The event to initialize the audio sessions.
class Init extends VoiceChangeDemoEvent {}

/// The event to dispose the audio sessions.
class Dispose extends VoiceChangeDemoEvent {}

/// The event to start changing the voice.
class StartChanging extends VoiceChangeDemoEvent {}

/// The event to stop changing the voice.
class StopChanging extends VoiceChangeDemoEvent {}

/// The event to allow toggling the voice changing.
class AllowToggle extends VoiceChangeDemoEvent {}

/// The event to disallow toggling the voice changing.
class DisallowToggle extends VoiceChangeDemoEvent {}
