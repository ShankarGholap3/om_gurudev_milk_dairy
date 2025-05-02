// lib/models/milk_entry.dart

class MilkEntry {
  int? id;
  String date;
  String session;
  int dairy;
  int farmer;
  double liters;

  MilkEntry({
    this.id,
    required this.date,
    required this.session,
    required this.dairy,
    required this.farmer,
    required this.liters,
  });

  Map<String, dynamic> toMap() {
    final map = <String, dynamic>{
      'date': date,
      'session': session,
      'dairy': dairy,
      'farmer': farmer,
      'liters': liters,
    };
    if (id != null) {
      map['id'] = id;
    }
    return map;
  }

  factory MilkEntry.fromMap(Map<String, dynamic> map) {
    return MilkEntry(
      id: map['id'],
      date: map['date'],
      session: map['session'],
      dairy: map['dairy'],
      farmer: map['farmer'],
      liters: (map['liters'] as num).toDouble(),
    );
  }
}