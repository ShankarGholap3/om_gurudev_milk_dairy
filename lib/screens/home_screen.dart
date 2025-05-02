// lib/screens/home_screen.dart

import 'package:flutter/material.dart';
import 'package:excel/excel.dart';
import 'dart:io';
import 'package:path_provider/path_provider.dart';
import 'package:path/path.dart' as path;
import '../services/db_service.dart';
import '../models/milk_entry.dart';

class HomeScreen extends StatefulWidget {
  @override
  _HomeScreenState createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  late DateTime _selectedDate;
  late String _session;
  int _dairy = 1;
  final _farmerCtrl = TextEditingController();
  final _litersCtrl = TextEditingController();
  double _totalMorning = 0.0;
  double _totalEvening = 0.0;

  @override
  void initState() {
    super.initState();
    _selectedDate = DateTime.now();
    _session = TimeOfDay.now().hour < 12 ? 'Morning' : 'Evening';
    _refreshTotals();
  }

  String _formatDate(DateTime dt) => "${dt.year}-${dt.month.toString().padLeft(2, '0')}-${dt.day.toString().padLeft(2, '0')}";

  Future<void> _refreshTotals() async {
    String dateKey = _formatDate(_selectedDate);
    double m = await DBService.getTotalFor(dateKey, 'Morning');
    double e = await DBService.getTotalFor(dateKey, 'Evening');
    setState(() {
      _totalMorning = m;
      _totalEvening = e;
    });
  }

  Future<void> _pickDate() async {
    final picked = await showDatePicker(
      context: context,
      initialDate: _selectedDate,
      firstDate: DateTime(2020),
      lastDate: DateTime(2100),
    );
    if (picked != null) {
      setState(() => _selectedDate = picked);
      _refreshTotals();
    }
  }

  Future<void> _saveEntry() async {
    final farmer = int.tryParse(_farmerCtrl.text.trim());
    final liters = double.tryParse(_litersCtrl.text.trim());
    if (farmer == null || liters == null || farmer <= 0 || farmer > 100 || liters <= 0) {
      ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text('अवैध इनपुट')));
      return;
    }
    final entry = MilkEntry(
      date: _formatDate(_selectedDate),
      session: _session,
      dairy: _dairy,
      farmer: farmer,
      liters: liters,
    );
    await DBService.insertEntry(entry);
    _farmerCtrl.clear();
    _litersCtrl.clear();
    _refreshTotals();
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text('नोंद सेव झाली')));
  }

  Future<void> _exportToExcel() async {
    final entries = await DBService.getAllEntries();
    if (entries.isEmpty) {
      ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text('डेटा उपलब्ध नाही')));
      return;
    }
    var excel = Excel.createExcel();
    final sheet = excel['Sheet1'];
    sheet.appendRow(['Date', 'Session', 'DairyNo', 'FarmerNo', 'Liters']);
    for (var e in entries) {
      sheet.appendRow([e['date'], e['session'], e['dairy'], e['farmer'], e['liters']]);
    }
    final fileBytes = excel.save();
    final dir = await getDownloadsDirectory();
    final filePath = path.join(dir!.path, 'OmGurudevMilkData.xlsx');
    final file = File(filePath);
    await file.writeAsBytes(fileBytes!);
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text('Excel फाईल तयार झाली')));
  }

  Future<void> _deleteDataInRange() async {
    DateTime? start = await showDatePicker(
      context: context,
      initialDate: DateTime.now(),
      firstDate: DateTime(2020),
      lastDate: DateTime.now(),
    );
    if (start == null) return;
    DateTime? end = await showDatePicker(
      context: context,
      initialDate: start,
      firstDate: start,
      lastDate: DateTime.now(),
    );
    if (end == null) return;

    bool? confirm = await showDialog(
      context: context,
      builder: (_) => AlertDialog(
        title: Text('पुष्टीकरण'),
        content: Text("${start.toLocal()} ते ${end.toLocal()} डेटा हटवायचा आहे का?"),
        actions: [
          TextButton(child: Text('नाही'), onPressed: () => Navigator.pop(context, false)),
          TextButton(child: Text('होय'), onPressed: () => Navigator.pop(context, true)),
        ],
      ),
    );

    if (confirm == true) {
      await DBService.deleteEntriesInRange(_formatDate(start), _formatDate(end));
      _refreshTotals();
      ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text('डेटा हटवण्यात आला')));
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text('Om Gurudev Milk Dairy')),
      body: Padding(
        padding: EdgeInsets.all(16),
        child: ListView(
          children: [
            Row(
              children: [
                Expanded(
                  child: InkWell(
                    onTap: _pickDate,
                    child: InputDecorator(
                      decoration: InputDecoration(labelText: 'दिनांक'),
                      child: Text("${_selectedDate.day}-${_selectedDate.month}-${_selectedDate.year}"),
                    ),
                  ),
                ),
                SizedBox(width: 10),
                Expanded(
                  child: Row(
                    children: [
                      Text('सत्र: '),
                      Radio<String>(
                        value: 'Morning',
                        groupValue: _session,
                        onChanged: (val) => setState(() => _session = val!),
                      ),
                      Text('सकाळ'),
                      Radio<String>(
                        value: 'Evening',
                        groupValue: _session,
                        onChanged: (val) => setState(() => _session = val!),
                      ),
                      Text('संध्याकाळ'),
                    ],
                  ),
                ),
              ],
            ),
            Row(
              children: [
                Text('डेअरी: '),
                DropdownButton<int>(
                  value: _dairy,
                  items: [1, 2, 3].map((e) => DropdownMenuItem(value: e, child: Text('$e'))).toList(),
                  onChanged: (val) => setState(() => _dairy = val!),
                ),
              ],
            ),
            TextField(
              controller: _farmerCtrl,
              decoration: InputDecoration(labelText: 'शेतकरी क्रमांक'),
              keyboardType: TextInputType.number,
            ),
            TextField(
              controller: _litersCtrl,
              decoration: InputDecoration(labelText: 'दूध (लिटर)'),
              keyboardType: TextInputType.numberWithOptions(decimal: true),
            ),
            SizedBox(height: 10),
            ElevatedButton(
              onPressed: _saveEntry,
              child: Text('जतन करा'),
            ),
            SizedBox(height: 20),
            Text('सकाळ एकूण: ${_totalMorning.toStringAsFixed(2)} लिटर'),
            Text('संध्याकाळ एकूण: ${_totalEvening.toStringAsFixed(2)} लिटर'),
            SizedBox(height: 20),
            ElevatedButton(
              onPressed: _exportToExcel,
              child: Text('Excel निर्यात'),
            ),
            ElevatedButton(
              onPressed: _deleteDataInRange,
              child: Text('डेटा हटवा'),
            ),
          ],
        ),
      ),
    );
  }
}
