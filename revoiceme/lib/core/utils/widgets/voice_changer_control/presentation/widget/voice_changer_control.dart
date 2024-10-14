import "package:flutter/material.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";

/// A circular button with an icon to toggle voice changing.
///
/// The button is disabled if the user does not have control.
/// The icon and background color change depending on whether voice changing is applied.
///
/// {@category Widget}
class VoiceChangerControl extends StatelessWidget {
  /// Creates a new [VoiceChangerControl] instance.
  const VoiceChangerControl({
    required this.toggleVoiceChanging,
    required this.userHasControl,
    required this.appliesVoiceChanging,
    required this.activeIcon,
    required this.inactiveIcon,
    required this.size,
    super.key,
  });

  /// Whether the user currently can toggle the voice changing.
  final bool userHasControl;

  /// Whether the voice changing is currently active.
  final bool appliesVoiceChanging;

  /// The icon to display when voice changing is active.
  final IconData activeIcon;

  /// The icon to display when voice changing is inactive.
  final IconData inactiveIcon;

  /// The size of the icon.
  final double size;

  /// The function to call for toggling the voice changing.
  final void Function() toggleVoiceChanging;

  @override
  Widget build(BuildContext context) {
    // Get colors
    const Color activeColor = Colors.red;
    const Color inactiveColor = Colors.lightGreen;
    const Color iconColor = Colors.white;

    // Determine the background color of the button
    final Color color = userHasControl
        ? appliesVoiceChanging
            ? activeColor
            : inactiveColor
        : Theme.of(context).disabledColor;

    return Ink(
      decoration: ShapeDecoration(
        color: color,
        shape: const CircleBorder(),
      ),
      child: IconButton(
        padding: const EdgeInsets.all(CustomThemeData.defaultPadding),
        icon: Icon(
          appliesVoiceChanging ? activeIcon : inactiveIcon,
          size: size,
          color: iconColor,
        ),
        onPressed: () {
          if (userHasControl) toggleVoiceChanging();
        },
      ),
    );
  }
}
