import datetime
import sys

class Date1:
    def init(self):
        self.data = []      # list of datetime.date objects
        self.passage = []   # list of license plates
        self.original = []  # list of original strings

    def parse_date(self, s: str):
        try:
            return datetime.datetime.strptime(s, "%Y-%m-%d").date()
        except ValueError:
            return None

    def extract(self, car_list):
        self.data.clear()
        self.passage.clear()
        self.original.clear()

        for item in car_list:
            parts = item.split(maxsplit=1)
            if len(parts) < 2:
                print(f"неверный формат строки: {item}")
                continue

            date_str, plate = parts
            parsed_date = self.parse_date(date_str)
            if parsed_date is not None:
                self.data.append(parsed_date)
                self.passage.append(plate)
                self.original.append(item)
            else:
                print(f"неверный формат даты в строке: {item}")

    def _apply_indices(self, indices):
        self.data = [self.data[i] for i in indices]
        self.passage = [self.passage[i] for i in indices]
        self.original = [self.original[i] for i in indices]

    def sort_by_date(self):
        indices = list(range(len(self.data)))
        indices.sort(key=lambda i: self.data[i])
        self._apply_indices(indices)

    def sort_by_plate_letter(self):
        indices = list(range(len(self.passage)))
        indices.sort(key=lambda i: self.passage[i][0].upper() if self.passage[i] else '')
        self._apply_indices(indices)

    def print(self):
        for d, plate in zip(self.data, self.passage):
            print(d.strftime("%Y-%m-%d"), plate)


# Данные
car_infos = [
    "2001-11-12 A000BC",
    "2004-03-10 E003EK",
    "2003-03-10 K009VO",
    "2005-03-10 K009VO",
    "2000-03-10 K009VO",
    "2000-03-15 K009VO",
    "2000-03-11 K009VO"
]

date1 = Date1()
date1.extract(car_infos)

print("Выберите способ сортировки:")
print("1 - по дате")
print("2 - по первой букве номерного знака")

while True:
    try:
        choice = input("Ваш выбор (1 или 2): ").strip()
        if choice == "1" or choice == "2":
            break
        else:
            print("Пожалуйста, введите 1 или 2.")
    except EOFError:
        print("\nВвод прерван. Завершение программы.")
        sys.exit(0)

if choice == "1":
    date1.sort_by_date()
elif choice == "2":
    date1.sort_by_plate_letter()

date1.print()