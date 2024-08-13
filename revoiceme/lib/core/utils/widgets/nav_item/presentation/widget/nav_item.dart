import "package:flutter/material.dart";
import "package:go_router/go_router.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_drawer/presentation/widget/revoiceme_drawer.dart";
import "package:revoiceme/core/utils/widgets/small_padder/presentation/widget/small_padder.dart";

/// A navigation item to be used inside a [ReVoiceMeDrawer].
///
/// {@category Widget}
class NavItem extends StatelessWidget {
  /// Creates a new [NavItem] instance.
  const NavItem({
    required this.route,
    required this.routeText,
    required this.icon,
    this.textColor,
    this.tileColor,
    super.key,
  });

  /// The route path to navigate to when the item is tapped.
  final String route;

  /// The text to display on the item.
  final String routeText;

  /// The icon to display left to the [routeText].
  final IconData icon;

  /// The text color of the item.
  final Color? textColor;

  /// The background color of the item.
  final Color? tileColor;

  @override
  Widget build(BuildContext context) {
    final ColorScheme colors = Theme.of(context).colorScheme;

    return ListTile(
      shape: RoundedRectangleBorder(
        borderRadius:
            BorderRadius.circular(CustomThemeData.defaultBorderRadius),
      ),
      title: Row(
        children: <Widget>[
          Icon(icon, color: colors.onSurface),
          const SmallPadder(),
          Text(
            routeText,
            style: TextStyle(color: colors.onSurface),
          ),
        ],
      ),
      onTap: () {
        Navigator.pop(context);
        context.go(route);
      },
      tileColor: tileColor,
    );
  }
}
