package ru.nsu.avdonina.model.parsing.config;

import ru.nsu.avdonina.model.parsing.PropertiesException;

import java.util.Properties;

public interface ConfigParser {
    Config parse(Properties properties) throws PropertiesException;
}
