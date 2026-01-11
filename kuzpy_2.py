import datetime

class Date1:
    def __init__(self):
        self.data = []      # list of date objects
        self.passage = []   # list of license plates
        self.original = []  # (не используется в print, но сохранена для совместимости)

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
            if len(parts) != 2:
                continue  # игнорируем строки с неправильным форматом (как в Lua без вывода ошибки)
            date_str, plate = parts
            parsed = self.parse_date(date_str)
            if parsed is not None:
                self.data.append(parsed)
                self.passage.append(plate)
                self.original.append(item)

    def sort_by_date(self):
        # Сортируем данные и passage параллельно по дате
        combined = sorted(zip(self.data, self.passage), key=lambda x: x[0])
        self.data, self.passage = zip(*combined) if combined else ([], [])
        # Преобразуем обратно в списки (zip возвращает кортежи)
        self.data = list(self.data)
        self.passage = list(self.passage)

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
date1.sort_by_date()
date1.print()
