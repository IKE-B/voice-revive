import "package:flutter/material.dart";
import "package:go_router/go_router.dart";
import "package:revoiceme/screens/main/presentation/screen/home_screen.dart";

/// The router for the different screens of the ReVoiceMe app.
///
/// {@category Other}
final GoRouter router = GoRouter(
  /// The routes to the different screens of the ReVoiceMe app.
  routes: <RouteBase>[
    GoRoute(
      path: Routes.home,
      builder: (BuildContext context, GoRouterState state) =>
          const HomeScreen(),
    ),
  ],
);

/// The route paths and route display names for the different screens of the ReVoiceMe app.
///
/// {@category Other}
class Routes {
  /// The route path to the home screen.
  static const String home = "/";

  /// The display name for the home screen.
  static const String homeText = "Hauptseite";
}
