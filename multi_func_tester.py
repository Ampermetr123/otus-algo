import folder_test as ft


class MultiFuncFolderTester:
    ''' Клсасс для прогона множества функций по тестам в папке
        Сохранет время отработки тестов и строит отчет в виде таблицы
    '''

    def __init__(self, test_list: dict, report_name: str, test_description_maker=None):
        self.test_list = test_list

        # 'test_name' -> [ dur_func1 , dur_func2, ... dur_funcN]
        self.table = {}

        self.result = {}  # 'test_name' -> [res_func1, res_func2 ... res_funcN]

        # 'test_name' -> [ make_description(input_args)]
        self.test_descriptions = {}
        self.current_func = 0
        self.report_name = report_name
        self.test_description_maker = (test_description_maker,)

    def store_callback(self, test_name, test_obj):
        if not test_name in self.table:
            self.table[test_name] = ['skipped']*(len(self.test_list))
            self.result[test_name] = [True]*(len(self.test_list))
            if (self.test_description_maker[0]):
                self.test_descriptions[test_name] = self.test_description_maker[0](
                    test_obj.input_args)

        self.table[test_name][self.current_func] = test_obj.times[test_name]
        self.result[test_name][self.current_func] = test_obj.test_result

    def do_test(self, folder, inp_loader, out_loader):
        self.table = {}
        self.current_func = 0
        self.test_descriptions = {}
        for func, rng in self.test_list:
            ft.run_test(folder, func, rng=rng,
                        inp_loader=inp_loader, out_loader=out_loader,
                        test_callback=self.store_callback)
            self.current_func += 1

    def write_report(self, f, header2: str = None):
        '''Вывод таблицы результатов'''
        if (header2):
            f.write(f"\n## {header2}\n\n")
        # header
        f.write('| test no ')
        for func, _rng in self.test_list:
            f.write(f'| {func.__name__} ')
            if desc := func.__doc__:
                f.write(f'<br> {desc} ')
        f.write('|\n')
        for i in range(len(self.test_list)+1):
            f.write('|:-------')
        f.write('|\n')

        # data
        for test_no, data in self.table.items():
            f.write(f'| {test_no} ')
            if test_no in self.test_descriptions:
                f.write(f'<br> {self.test_descriptions[test_no]}')
            for index, x in enumerate(data):
                if isinstance(x, float):
                    f.write(f'| {x:.3f} ')
                else:
                    f.write(f'| {x}')
                if self.result[test_no][index] == False:
                    f.write(f' <br> __FAILED__ ')
            f.write('|\n')
        f.flush()

    def write_report_tr(self, f, header2: str = None):
        '''Вывод 2 таблицы результатов'''
        if (header2):
            f.write(f"\n## {header2}\n\n")
        # header
        f.write('| function ')
        for test_name in self.result.keys():
            f.write(f'| {test_name} ')
            if test_name in self.test_descriptions:
                f.write(f'<br> {self.test_descriptions[test_name]}')
        f.write('|\n')
        for i in range(len(self.result)+1):
            f.write('|:-------')
        f.write('|\n')

        # data
        for i, (func, _rng) in enumerate(self.test_list):
            f.write(f'| {func.__name__} ')
            if desc := func.__doc__:
                f.write(f'<br> {desc} ')

            for test_name, data in self.table.items():
                x = data[i]
                if isinstance(x, float):
                    f.write(f'| {x:.3f} ')
                else:
                    f.write(f'| {x}')
                if self.result[test_name][i] == False:
                    f.write(f' <br> __FAILED__ ')
            f.write('|\n')
        f.flush()
