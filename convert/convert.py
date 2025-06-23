import json
import csv

# Load JSON data dari file
with open('C:\\Users\\User\\Documents\\Non-University\\Project\\tetes\\convert\\data.json', 'r') as f:
    data = json.load(f)

rows = []

for key, entry in data.items():
    row = {
        'id': key,
        'timestamp': entry.get('timestamp'),
        'topic': entry.get('topic'),
    }
    
    # Decode string JSON di 'value'
    value_str = entry.get('value', '{}')
    try:
        value_data = json.loads(value_str)
    except json.JSONDecodeError:
        value_data = {}
    
    # Gabungkan dengan row utama
    row.update(value_data)
    
    rows.append(row)

# Simpan ke file CSV
with open('output.csv', 'w', newline='') as f:
    fieldnames = rows[0].keys()
    writer = csv.DictWriter(f, fieldnames=fieldnames)
    writer.writeheader()
    writer.writerows(rows)

print("Export selesai: output.csv")
