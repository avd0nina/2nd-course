package ru.nsu.avdonina.model.parsing.limits;

import ru.nsu.avdonina.model.parsing.PropertiesException;

import java.util.Properties;

public interface LimitsParser {
    Limits parse(Properties properties) throws PropertiesException;
}
