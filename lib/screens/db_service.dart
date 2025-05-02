// lib/services/db_service.dart

import 'package:sqflite/sqflite.dart';
import 'package:path/path.dart';
import '../models/milk_entry.dart';

class DBService {
  static Database? _db;

  static Future<void> init() async {
    final dbPath = await getDatabasesPath();
    final path = join(dbPath, 'milk_collection.db');
    _db = await openDatabase(
      path,
      version: 1,
      onCreate: (db, version) async {
        await db.execute(
          'CREATE TABLE entries ('
          'id INTEGER PRIMARY KEY AUTOINCREMENT, '
          'date TEXT, '
          'session TEXT, '
          'dairy INTEGER, '
          'farmer INTEGER, '
          'liters REAL'
          ')'
        );
      },
    );
  }

  static Future<int> insertEntry(MilkEntry entry) async {
    return await _db!.insert('entries', entry.toMap());
  }

  static Future<double> getTotalFor(String date, String session) async {
    final result = await _db!.rawQuery(
      'SELECT COALESCE(SUM(liters), 0) AS total '
      'FROM entries WHERE date = ? AND session = ?',
      [date, session],
    );
    if (result.isNotEmpty) {
      return (result.first['total'] as num).toDouble();
    }
    return 0.0;
  }

  static Future<List<Map<String, dynamic>>> getAllEntries() async {
    return await _db!.query('entries', orderBy: 'date ASC, session DESC');
  }

  static Future<int> deleteEntriesInRange(String startDate, String endDate) async {
    return await _db!.delete(
      'entries',
      where: 'date >= ? AND date <= ?',
      whereArgs: [startDate, endDate],
    );
  }
}