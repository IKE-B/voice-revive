import "package:flutter/material.dart";
import "package:revoiceme/core/routers/router.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";
import "package:revoiceme/core/utils/widgets/nav_item/presentation/widget/nav_item.dart";

/// The drawer for the ReVoiceMe app for navigating between screens.
///
/// {@category Widget}
class ReVoiceMeDrawer extends StatelessWidget {
  /// Creates a new [ReVoiceMeDrawer] instance.
  const ReVoiceMeDrawer({this.cleanup, super.key});

  /// A cleanup method that is called when navigating away from the screen via the drawer.
  /// It is called before the new screen is built (and before the current screen is disposed).
  final void Function()? cleanup;

  @override
  Widget build(BuildContext context) => Drawer(
        child: ListView(
          children: <Widget>[
            const SizedBox(
              height: CustomThemeData.drawerHeaderHeight,
              child: DrawerHeader(
                child: Text("Übersicht"),
              ),
            ),
            Row(
              children: <Widget>[
                Expanded(
                  // ListView(
                  child: Container(
                    padding: const EdgeInsets.symmetric(
                      horizontal: CustomThemeData.defaultPadding,
                    ),
                    child: Column(
                      children: List<NavItem>.generate(
                        routes.length,
                        (int index) => NavItem(
                          routeText: routes.values.elementAt(index).text,
                          route: routes.values.elementAt(index).path,
                          icon: routes.values.elementAt(index).icon,
                          cleanup: cleanup,
                        ),
                      ),
                    ),
                  ),
                ),
              ],
            ),
          ],
        ),
      );
}
