package ru.nsu.avdonina.view;

import ru.nsu.avdonina.Main;
import ru.nsu.avdonina.model.parsing.config.Config;
import ru.nsu.avdonina.model.parsing.limits.Limits;

import javax.imageio.ImageIO;
import javax.swing.*;
import javax.swing.event.ChangeListener;
import javax.swing.table.DefaultTableCellRenderer;
import java.awt.*;
import java.awt.event.WindowListener;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.InputStream;
import java.util.logging.Logger;

public class DefaultFrameFactory implements AbstractFrameFactory {
    private static final String ICON_NAME = "default_icon.png";

    private static final Font LABELS_FONT = new Font(Font.SERIF, Font.PLAIN, 30);
    private static final Font TABLE_FONT = new Font(Font.SERIF, Font.PLAIN, 18);
    private static final Font SLIDER_TITLE_FONT = new Font(Font.SERIF, Font.PLAIN, 20);
    private static final Font SLIDER_UNITS_FONT = new Font(Font.SERIF, Font.PLAIN, 12);

    private static final Dimension FRAME_DIMENSION = new Dimension(1500, 600);

    private static final DefaultTableCellRenderer cellRenderer = new DefaultTableCellRenderer();

    private static final int TABLE_HEADER_HEIGHT = 60;
    private static final int TABLE_ROW_HEIGHT = 75;
    private static final int SECTION_TITLE_WIDTH = 300;
    private static final int SLIDERS_STRUT = 60;

    private static final String DEFAULT_TABLE_VALUE = "0";

    private static final int SLIDER_TICKS_COUNT = 3;

    private static final String TOTAL_SECTION_TITLE = "Total";
    private static final String STORED_SECTION_TITLE = "Stored";
    private static final String WORKERS_COUNT_SECTION_TITLE = "Number";
    private static final String WORK_TIME_SECTION_TITLE = "Work time (ms)";
    private static final String SECTION_TITLE_OFFSET = "  ";

    private static final String FRAME_TITLE = "FactoryEmulator";

    private static final String[] SLIDER_TITLES = new String[]{
            "Body suppliers",
            "Engine suppliers",
            "Accessory suppliers",
            "Factory workers",
            "Dealers"
    };

    private static final String[] TOTAL_TABLE_HEADER_COLUMNS = new String[]{
            "Bodies",
            "Engines",
            "Accessories",
            "Assembled",
            "Sold"
    };

    private static final String[] STORED_TABLE_HEADER_COLUMNS = new String[]{
            "Bodies",
            "Engines",
            "Accessories",
            "Cars",
            "Tasks"
    };

    static {
        cellRenderer.setHorizontalAlignment(SwingConstants.CENTER);
    }

    private static JPanel createTableSection(JLabel labelWithSectionTitle, JTable table) {
        final JPanel section = new JPanel();
        section.setLayout(new BoxLayout(section, BoxLayout.X_AXIS)); // горизонтальное выравнивание
        section.add(labelWithSectionTitle);
        section.add(new JScrollPane(table));
        return section;
    }

    private static JPanel createSlidersSection(JLabel labelWithSectionTitle, JSlider[] sliders) {
        final JPanel section = new JPanel();
        section.setLayout(new BoxLayout(section, BoxLayout.X_AXIS));
        section.add(labelWithSectionTitle);
        addSlidersToPanel(section, sliders); // добавляем слайдеры
        return section;
    }

    private static JTable createEmptyTable(String[] columnsHeaders) { // создаёт пустую таблицy
        final Object[][] data = new Object[1][columnsHeaders.length]; // двухмерный массив data, который будет содержать данные для таблицы
        for (int i = 0; i < columnsHeaders.length; i++) {
            data[0][i] = DEFAULT_TABLE_VALUE; // В каждой ячейке массива (строка 0, все столбцы) записывается значение DEFAULT_TABLE_VALUE, которое равно "0"
        }
        final JTable ret = new JTable(data, columnsHeaders) {
            @Override
            public boolean isCellEditable(int row, int column) { // чтобы все ячейки таблицы были не редактируемыми
                return false;
            }
        };
        ret.getTableHeader().setReorderingAllowed(false); // отключает возможность перетаскивать заголовки столбцов
        ret.getTableHeader().setFont(TABLE_FONT); // Устанавливается шрифт для заголовков таблицы
        ret.setFont(TABLE_FONT); // устанавливает шрифт для самой таблицы
        ret.setFillsViewportHeight(true); // делает так, чтобы таблица заполнила весь доступный вертикальный размер
        ret.setRowHeight(TABLE_ROW_HEIGHT); // задает высоту каждой строки таблицы
        ret.getTableHeader().setPreferredSize(new Dimension(0, TABLE_HEADER_HEIGHT)); // устанавливает предпочтительную высоту для заголовка таблицы
        for (int i = 0; i < columnsHeaders.length; i++) {
            ret.getColumnModel().getColumn(i).setCellRenderer(cellRenderer); // устанавливает выравнивание содержимого ячеек по центру
        }
        return ret;
    }

    private static JSlider[] createProductivitySliders(Config config,
                                                       Limits limits,
                                                       ChangeListener bodySupplyTimeListener,
                                                       ChangeListener engineSupplyTimeListener,
                                                       ChangeListener accessorySupplyTimeListener,
                                                       ChangeListener carAssemblingTimeListener,
                                                       ChangeListener carRequestTimeListener) {

        final JSlider bodySupplyTimeSlider = createHorizontalSlider( // создается слайдер
                bodySupplyTimeListener, // слушатель событий для слайдера. Он будет реагировать на изменения значений
                limits.minBodySupplyTimeMs(),
                limits.maxBodySupplyTimeMs(),
                config.bodySupplyTimeMs() // начальное значение слайдера, полученное из объекта конфигурации
        );
        final JSlider engineSupplyTimeSlider = createHorizontalSlider(
                engineSupplyTimeListener,
                limits.minEngineSupplyTimeMs(),
                limits.maxEngineSupplyTimeMs(),
                config.engineSupplyTimeMs()
        );
        final JSlider accessorySupplyTimeSlider = createHorizontalSlider(
                accessorySupplyTimeListener,
                limits.minAccessorySupplyTimeMs(),
                limits.maxAccessorySupplyTimeMs(),
                config.accessorySupplyTimeMs()
        );
        final JSlider carAssemblingTimeSlider = createHorizontalSlider(
                carAssemblingTimeListener,
                limits.minCarAssemblingTimeMs(),
                limits.maxCarAssemblingTimeMs(),
                config.carAssemblingTimeMs()
        );
        final JSlider carRequestTimeSlider = createHorizontalSlider(
                carRequestTimeListener,
                limits.minCarRequestTimeMs(),
                limits.maxCarRequestTimeMs(),
                config.carRequestTimeMs()
        );

        return new JSlider[]{ // массив слайдеров
                bodySupplyTimeSlider,
                engineSupplyTimeSlider,
                accessorySupplyTimeSlider,
                carAssemblingTimeSlider,
                carRequestTimeSlider
        };
    }

    private static JSlider[] createThreadNumberSliders(Config config,
                                                       Limits limits,
                                                       ChangeListener bodySuppliersCountListener,
                                                       ChangeListener engineSuppliersCountListener,
                                                       ChangeListener accessorySuppliersCountListener,
                                                       ChangeListener workersCountListener,
                                                       ChangeListener dealersCountListener) {

        final JSlider bodySuppliersCountSlider = createHorizontalSlider(
                bodySuppliersCountListener,
                limits.minBodySuppliersCount(),
                limits.maxBodySuppliersCount(),
                config.bodySuppliersCount()
        );
        final JSlider engineSuppliersCountSlider = createHorizontalSlider(
                engineSuppliersCountListener,
                limits.minEngineSuppliersCount(),
                limits.maxEngineSuppliersCount(),
                config.engineSuppliersCount()
        );
        final JSlider accessorySuppliersCountSlider = createHorizontalSlider(
                accessorySuppliersCountListener,
                limits.minAccessorySuppliersCount(),
                limits.maxAccessorySuppliersCount(),
                config.accessorySuppliersCount()
        );
        final JSlider workersCountSlider = createHorizontalSlider(
                workersCountListener,
                limits.minWorkersCount(),
                limits.maxWorkersCount(),
                config.workersCount()
        );
        final JSlider dealersCountSlider = createHorizontalSlider(
                dealersCountListener,
                limits.minDealersCount(),
                limits.maxDealersCount(),
                config.dealersCount()
        );

        return new JSlider[]{
                bodySuppliersCountSlider,
                engineSuppliersCountSlider,
                accessorySuppliersCountSlider,
                workersCountSlider,
                dealersCountSlider
        };
    }

    private static void addSlidersToPanel(JPanel jPanel, JSlider[] sliders) {
        for (int i = 0; i < sliders.length; i++) { // Цикл по всем слайдерам
            final JPanel tmp = new JPanel(); // Создание временной панели для каждого слайдера
            tmp.setLayout(new GridLayout(2, 1)); // менеджер компоновки GridLayout с двумя строками и одной колонкой (слайдер будет под меткой)
            final JLabel jLabel = new JLabel(SLIDER_TITLES[i], SwingConstants.CENTER); // Создаётся метка jLabel, текст которой берётся из массива SLIDER_TITLES по индексу i. Текст метки — это название параметра
            jLabel.setFont(SLIDER_TITLE_FONT);
            tmp.add(jLabel, Component.RIGHT_ALIGNMENT); // добавляется метка
            tmp.add(sliders[i]); // добавляется слайдер
            jPanel.add(tmp); // Временная панель tmp добавляется на основную панель jPanel
            jPanel.add(Box.createHorizontalStrut(SLIDERS_STRUT));
        }
    }

    private static JSlider createHorizontalSlider(ChangeListener changeListener,
                                                  int minValue,
                                                  int maxValue,
                                                  int startValue) {
        final JSlider ret = new JSlider(); // Создание нового слайдера
        configureSlider(ret, changeListener, minValue, maxValue, startValue); // Настройка слайдера
        return ret;
    }

    private static void configureSlider(JSlider slider, // для настройки параметров слайдера
                                        ChangeListener changeListener,
                                        int minValue,
                                        int maxValue,
                                        int startValue) {
        final int step = (maxValue - minValue) / SLIDER_TICKS_COUNT; // Вычисление шага для тиков
        slider.setMinimum(minValue);
        slider.setMaximum(maxValue);
        slider.setValue(startValue); // Устанавливаем начальное значение слайдера
        slider.setOrientation(SwingConstants.HORIZONTAL);
        slider.setPaintTicks(true); // Отображаем тики (отметки на слайдере)
        slider.setPaintLabels(true); // Отображаем метки с подписями
        slider.setMajorTickSpacing(step); // Устанавливаем шаг для основных меток
        slider.setMinorTickSpacing(step); // Устанавливаем шаг для второстепенных меток
        slider.setFont(SLIDER_UNITS_FONT);
        slider.addChangeListener(changeListener); // Добавляем слушатель изменений слайдера
    }

    private static JLabel createLabelWithSectionTitle(String sectionTitle) {
        final JLabel ret = new JLabel(sectionTitle); // Создание метки с текстом
        ret.setFont(LABELS_FONT); // Установка шрифта
        ret.setPreferredSize(new Dimension(SECTION_TITLE_WIDTH, 0));
        ret.setMinimumSize(new Dimension(SECTION_TITLE_WIDTH, 0));
        return ret;
    }

    private static void loadApplicationIcon(JFrame frame, Logger logger) { // для загрузки и установки иконки приложения
        try (InputStream inputStream = Main.class.getClassLoader().getResourceAsStream(ICON_NAME)) {
            if (inputStream == null) {
                logger.warning("Icon not found");
                return;
            }
            final BufferedImage bufferedImage = ImageIO.read(inputStream);
            final Image image = Toolkit.getDefaultToolkit().createImage(bufferedImage.getSource());
            frame.setIconImage(image);
        } catch (IOException exception) {
            logger.warning("Couldn't read an icon");
        }
    }

    @Override
    public Frame createMainFrame(Config config,
                                 Limits limits,
                                 Logger logger,
                                 ChangeListener bodySuppliersCountListener,
                                 ChangeListener engineSuppliersCountListener,
                                 ChangeListener accessorySuppliersCountListener,
                                 ChangeListener workersCountListener,
                                 ChangeListener dealersCountListener,
                                 ChangeListener bodySupplyTimeListener,
                                 ChangeListener engineSupplyTimeListener,
                                 ChangeListener accessorySupplyTimeListener,
                                 ChangeListener carAssemblingTimeListener,
                                 ChangeListener carRequestTimeListener,
                                 WindowListener windowListener) {
        final JTable suppliedTable = createEmptyTable(TOTAL_TABLE_HEADER_COLUMNS); // Создание таблиц
        final JTable storedTable = createEmptyTable(STORED_TABLE_HEADER_COLUMNS);
        final Frame ret = new DefaultFrame(suppliedTable, storedTable); // Создается фрейм, в который добавляются таблицы
        ret.setVisible(true);
        ret.setLayout(new GridLayout(4, 1)); // расположение компонентов в 4 строки и 1 колонку
        ret.setPreferredSize(FRAME_DIMENSION); // размер окна
        ret.setResizable(false); // не будет изменять свой размер
        ret.addWindowListener(windowListener); // слушатель событий окна
        final JLabel totalSectionLabel = createLabelWithSectionTitle(SECTION_TITLE_OFFSET + TOTAL_SECTION_TITLE); // Создание меток для разделов
        final JLabel storedSectionLabel = createLabelWithSectionTitle(SECTION_TITLE_OFFSET + STORED_SECTION_TITLE);
        final JLabel threadNumberSectionLabel = createLabelWithSectionTitle(SECTION_TITLE_OFFSET + WORKERS_COUNT_SECTION_TITLE);
        final JLabel productivitySectionLabel = createLabelWithSectionTitle(SECTION_TITLE_OFFSET + WORK_TIME_SECTION_TITLE);
        final JSlider[] threadsNumberSliders = createThreadNumberSliders( // Создание слайдеров
                config,
                limits,
                bodySuppliersCountListener,
                engineSuppliersCountListener,
                accessorySuppliersCountListener,
                workersCountListener,
                dealersCountListener
        );
        final JSlider[] productivitySliders = createProductivitySliders(
                config,
                limits,
                bodySupplyTimeListener,
                engineSupplyTimeListener,
                accessorySupplyTimeListener,
                carAssemblingTimeListener,
                carRequestTimeListener
        );
        final JPanel totalSection = createTableSection(totalSectionLabel, suppliedTable); // Создание секций для таблиц и слайдеров
        final JPanel storedSection = createTableSection(storedSectionLabel, storedTable);
        final JPanel productivitySection = createSlidersSection(threadNumberSectionLabel, threadsNumberSliders);
        final JPanel threadsNumberSection = createSlidersSection(productivitySectionLabel, productivitySliders);
        ret.add(totalSection); // Добавление секций в фрейм
        ret.add(storedSection);
        ret.add(productivitySection);
        ret.add(threadsNumberSection);
        loadApplicationIcon(ret, logger);
        ret.setTitle(FRAME_TITLE);
        return ret;
    }
}
