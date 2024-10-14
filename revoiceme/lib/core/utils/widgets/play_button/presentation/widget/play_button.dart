import "package:flutter/material.dart";
import "package:flutter_bloc/flutter_bloc.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";
import "package:revoiceme/core/utils/widgets/voice_change_demo/presentation/widget/voice_change_demo_bloc.dart";
import "package:revoiceme/core/utils/widgets/voice_change_demo/presentation/widget/voice_change_demo_event.dart";
import "package:revoiceme/core/utils/widgets/voice_change_demo/presentation/widget/voice_change_demo_state.dart";

/// A circular button with an icon to toggle voice changing.
///
/// Depends on [VoiceChangeDemo].
/// The [PlayButton] is disabled while the user is not allowed to toggle voice changing.
/// The icon and background color change depending on whether
/// the user is allowed to toggle the voice changing and whether voice changing is active.
///
/// {@category Widget}
/// {@category Global}
class PlayButton extends StatelessWidget {
  /// Creates a new [PlayButton] instance.
  const PlayButton({
    required this.size,
    super.key,
  });

  /// The size of the icon.
  final double size;

  /// Function to call when the button is pressed.
  /// Attempts to toggle the voice changing state depending on whether
  /// the user is allowed to toggle the voice changing and whether voice changing is active.
  void onPressed(BuildContext context, VoiceChangeDemoState state) {
    if (state.isToggleAllowed) {
      if (state.isChangingVoice) {
        context.read<VoiceChangeDemo>().add(StopChanging());
      } else {
        context.read<VoiceChangeDemo>().add(StartChanging());
      }
    }
  }

  /// Infer the background color of the player depending on whether
  /// the user is allowed to toggle the related action's state and whether the related action is active.
  Color inferPlayerColor(BuildContext context, VoiceChangeDemoState state) =>
      state.isToggleAllowed
          ? state.isChangingVoice
              ? Colors.red
              : Colors.lightGreen
          : Theme.of(context).disabledColor;

  /// Infer the icon to show for indicating the related action's state.
  IconData inferPlayerIcon(VoiceChangeDemoState state) =>
      state.isChangingVoice ? Icons.pause_rounded : Icons.play_arrow_rounded;

  @override
  Widget build(BuildContext context) =>
      BlocBuilder<VoiceChangeDemo, VoiceChangeDemoState>(
        builder: (BuildContext context, VoiceChangeDemoState state) => Ink(
          decoration: ShapeDecoration(
            color: inferPlayerColor(context, state),
            shape: const CircleBorder(),
          ),
          child: IconButton(
            padding: const EdgeInsets.all(CustomThemeData.defaultPadding),
            icon: Icon(
              inferPlayerIcon(state),
              size: size,
              color: Colors.white,
            ),
            onPressed: () => onPressed(context, state),
          ),
        ),
      );
}
