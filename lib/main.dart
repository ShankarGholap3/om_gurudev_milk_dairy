// lib/main.dart

import 'package:flutter/material.dart';
import 'package:flutter_localizations/flutter_localizations.dart';
import 'screens/home_screen.dart';
import 'services/db_service.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await DBService.init();
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Om Gurudev Milk Dairy',
      debugShowCheckedModeBanner: false,
      locale: Locale('mr', 'IN'),
      supportedLocales: [
        Locale('mr', 'IN'),
        Locale('en', 'US'),
      ],
      localizationsDelegates: [
        GlobalMaterialLocalizations.delegate,
        GlobalWidgetsLocalizations.delegate,
      ],
      home: HomeScreen(),
    );
  }
}