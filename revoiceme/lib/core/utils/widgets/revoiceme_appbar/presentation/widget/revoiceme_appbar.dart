import "package:flutter/material.dart";
import "package:flutter_bloc/flutter_bloc.dart";
import "package:revoiceme/core/routers/router.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";
import "package:revoiceme/core/utils/widgets/voice_change_demo/presentation/widget/voice_change_demo_bloc.dart";
import "package:revoiceme/core/utils/widgets/voice_change_demo/presentation/widget/voice_change_demo_event.dart";
import "package:revoiceme/core/utils/widgets/voice_change_demo/presentation/widget/voice_change_demo_state.dart";

/// The app bar for the ReVoiceMe app.
///
/// Depends on [VoiceChangeDemo].
/// The [ReVoiceMeAppBar] is a custom app bar consisting of multiple components.
/// The title is automatically inferred from the current route.
/// The app bar provides an icon button to toggle voice changing.
/// The icon changes depending on whether
/// the user is allowed to toggle the voice changing and whether voice changing is active.
///
/// {@category Widget}
/// {@category Global}
class ReVoiceMeAppBar extends StatelessWidget implements PreferredSizeWidget {
  /// Creates a new [ReVoiceMeAppBar] instance.
  const ReVoiceMeAppBar({
    super.key,
  });

  /// Change the app bar height to be more visually appealing.
  @override
  Size get preferredSize => const Size.fromHeight(kToolbarHeight);

  /// Infer the title of the app bar from the current route.
  String inferTitle(BuildContext context) => routes.values
      .firstWhere(
        (({IconData icon, String path, String text}) element) =>
            element.path == ModalRoute.of(context)?.settings.name,
      )
      .text;

  /// Infer the icon to show for indicating the voice changing state.
  IconData inferIndicatorIcon(VoiceChangeDemoState state) =>
      state.isChangingVoice ? Icons.mic : Icons.mic_off;

  /// Function to call when the microphone icon is pressed.
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

  /// Infer the color of the indicator depending on
  /// whether the user is allowed to toggle the voice changing.
  Color inferIndicatorColor(BuildContext context, VoiceChangeDemoState state) {
    final ColorScheme colors = Theme.of(context).colorScheme;
    return state.isToggleAllowed
        ? colors.onSurface
        : Theme.of(context).disabledColor;
  }

  @override
  Widget build(BuildContext context) {
    // Get color palette
    final ColorScheme colors = Theme.of(context).colorScheme;

    return BlocBuilder<VoiceChangeDemo, VoiceChangeDemoState>(
      builder: (BuildContext context, VoiceChangeDemoState state) => AppBar(
        title: Text(inferTitle(context)),
        backgroundColor: colors.primary,
        actions: <Widget>[
          IconButton(
            icon: Icon(inferIndicatorIcon(state)),
            onPressed: () => onPressed(context, state),
          ),
        ],
        iconTheme: IconThemeData(
          color: colors.onSurface,
          size: CustomThemeData.defaultIconSize,
        ),
        actionsIconTheme: IconThemeData(
          color: inferIndicatorColor(context, state),
          size: CustomThemeData.defaultIconSize,
        ),
      ),
    );
  }
}
