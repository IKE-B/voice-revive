import "package:equatable/equatable.dart";
import "package:flutter/material.dart";

/// The state model of [VoiceChangeDemo].
///
/// {@category Other}
@immutable
class VoiceChangeDemoState extends Equatable {
  /// Creates a new instance of [VoiceChangeDemoState].
  const VoiceChangeDemoState({
    this.isInitialized = false,
    this.isChangingVoice = false,
    this.isToggleAllowed = true,
  });

  /// Whether the audio sessions are initialized.
  final bool isInitialized;

  /// Whether the voice is being changed.
  final bool isChangingVoice;

  /// Whether the user is allowed to toggle voice changing.
  final bool isToggleAllowed;

  @override
  List<Object> get props =>
      <Object>[isInitialized, isChangingVoice, isToggleAllowed];
}
