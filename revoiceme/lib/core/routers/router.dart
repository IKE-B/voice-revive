import "package:flutter/material.dart";
import "package:go_router/go_router.dart";
import "package:revoiceme/screens/main/presentation/screen/configurator_screen.dart";
import "package:revoiceme/screens/main/presentation/screen/home_screen.dart";

/// The router for the different screens of the ReVoiceMe app.
///
/// {@category Other}
final GoRouter router = GoRouter(
  /// The routes to the different screens of the ReVoiceMe app.
  routes: <RouteBase>[
    GoRoute(
      path: routes["home"]!.path,
      builder: (BuildContext context, GoRouterState state) =>
          const HomeScreen(),
    ),
    GoRoute(
      path: routes["configurator"]!.path,
      builder: (BuildContext context, GoRouterState state) =>
          const ConfiguratorScreen(),
    ),
  ],
);

/// Static data about the routes of the ReVoiceMe app.
/// Contains the path, text and icon for each route.
///
/// {@category Other}
const Map<String, ({String path, String text, IconData icon})> routes =
    <String, ({String path, String text, IconData icon})>{
  "home": (path: "/", text: "Hauptseite", icon: Icons.home),
  "configurator": (
    path: "/configurator",
    text: "Konfiguration",
    icon: Icons.settings
  ),
};
