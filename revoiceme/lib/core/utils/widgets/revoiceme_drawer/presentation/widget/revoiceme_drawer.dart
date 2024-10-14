import "package:flutter/material.dart";
import "package:revoiceme/core/routers/router.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";
import "package:revoiceme/core/utils/widgets/nav_item/presentation/widget/nav_item.dart";

/// The drawer for the ReVoiceMe app.
/// 
/// {@category Widget}
class ReVoiceMeDrawer extends StatelessWidget {
  /// Creates a new [ReVoiceMeDrawer] instance.
  const ReVoiceMeDrawer(this.selectedRoute, {super.key});

  /// The currently active screen and its associated route.
  final String selectedRoute;

  @override
  Widget build(BuildContext context) {
    const Color? unselectedColor = null;
    final Color selectedColor = Theme.of(context).colorScheme.primaryContainer;

    return Drawer(
      child: ListView(
        children: <Widget>[
          const SizedBox(
            height: CustomThemeData.drawerHeaderHeight,
            child: DrawerHeader(
              child: Text("Übersicht"),
            ),
          ),
          Expanded(
            // ListView(
            child: Container(
              padding: const EdgeInsets.symmetric(
                horizontal: CustomThemeData.defaultPadding,
              ),
              child: Column(
                children: <Widget>[
                  NavItem(
                    routeText: Routes.homeText,
                    route: Routes.home,
                    icon: Icons.home,
                    tileColor: selectedRoute == Routes.home
                        ? selectedColor
                        : unselectedColor,
                  ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }
}
