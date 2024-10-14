import "package:flutter/material.dart";
import "package:go_router/go_router.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";
import "package:revoiceme/core/utils/widgets/revoiceme_drawer/presentation/widget/revoiceme_drawer.dart";
import "package:revoiceme/core/utils/widgets/small_padder/presentation/widget/small_padder.dart";

/// A navigation item to be used inside a [ReVoiceMeDrawer].
///
/// A [NavItem] can be tapped to navigate to a different screen.
/// It is able to perform [cleanup] before navigating to the new screen.
/// Automatically highlights the item if the current route matches the item's route.
///
/// {@category Widget}
class NavItem extends StatelessWidget {
  /// Creates a new [NavItem] instance.
  const NavItem({
    required this.route,
    required this.routeText,
    required this.icon,
    this.cleanup,
    super.key,
  });

  /// The route path to navigate to when the item is tapped.
  final String route;

  /// The text to display on the item.
  final String routeText;

  /// The icon to display left to the [routeText].
  final IconData icon;

  /// A cleanup method that is called when navigating away from the screen via the item.
  /// It is called before the new screen is built (and before the current screen is disposed).
  final void Function()? cleanup;

  /// Performs [cleanup] and then starts the navigation to the item's route.
  void onTap(BuildContext context) {
    cleanup?.call();
    Navigator.pop(context);
    context.go(route);
  }

  /// Infer the color of the tile depending on whether the [currenRoute] matches the item's [route].
  Color? inferTileColor(BuildContext context) {
    final String? currentRoute = ModalRoute.of(context)?.settings.name;

    return currentRoute != null && currentRoute == route
        ? Theme.of(context).colorScheme.primaryContainer
        : null;
  }

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
      onTap: () => onTap(context),
      tileColor: inferTileColor(context),
    );
  }
}
