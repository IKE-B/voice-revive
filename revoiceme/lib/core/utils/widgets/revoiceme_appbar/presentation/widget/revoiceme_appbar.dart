import "package:flutter/material.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";

/// The app bar for the ReVoiceMe app.
///
/// The app bar provides an icon button to toggle voice changing.
/// The icon changes depending on whether voice changing is applied.
///
/// {@category Widget}
class ReVoiceMeAppBar extends StatelessWidget implements PreferredSizeWidget {
  /// Creates a new [ReVoiceMeAppBar] instance.
  const ReVoiceMeAppBar({
    required this.title,
    required this.userHasControl,
    required this.appliesVoiceChanging,
    required this.toggleVoiceChanging,
    super.key,
  });

  /// The text to display in the app bar.
  final String title;

  /// Whether the user currently can toggle the voice changing.
  final bool userHasControl;

  /// Whether the voice changing is currently active.
  final bool appliesVoiceChanging;

  /// The function to call for toggling the voice changing.
  final VoidCallback toggleVoiceChanging;

  /// Change the app bar height to be more visually appealing.
  @override
  Size get preferredSize => const Size.fromHeight(kToolbarHeight);

  @override
  Widget build(BuildContext context) {
    // Get icons
    const IconData activeIcon = Icons.mic;
    const IconData inactiveIcon = Icons.mic_off;

    // Get colors
    final ColorScheme colors = Theme.of(context).colorScheme;
    final Color enabledColor = colors.onSurface;
    final Color disabledColor = Theme.of(context).disabledColor;

    return AppBar(
      title: Text(title),
      backgroundColor: colors.primary,
      actions: <Widget>[
        IconButton(
          icon: Icon(
            appliesVoiceChanging ? activeIcon : inactiveIcon,
          ),
          onPressed: () {
            if (userHasControl) toggleVoiceChanging();
          },
        ),
      ],
      iconTheme: IconThemeData(
        color: colors.onSurface,
        size: CustomThemeData.defaultIconSize,
      ),
      actionsIconTheme: IconThemeData(
        color: userHasControl ? enabledColor : disabledColor,
        size: CustomThemeData.defaultIconSize,
      ),
    );
  }
}
