import "package:flutter/material.dart";
import "package:revoiceme/core/routers/router.dart";
import "package:revoiceme/core/utils/styles/default_theme.dart";

void main() {
  runApp(const ReVoiceMe());
}

/// The main widget of the ReVoiceMe app.
///
/// {@category Other}
class ReVoiceMe extends StatelessWidget {
  /// Creates a new [ReVoiceMe] instance.
  const ReVoiceMe({super.key});

  @override
  Widget build(BuildContext context) => MaterialApp.router(
        title: "ReVoiceMe",
        theme: buildThemeData(context),
        routerConfig: router,
      );
}
