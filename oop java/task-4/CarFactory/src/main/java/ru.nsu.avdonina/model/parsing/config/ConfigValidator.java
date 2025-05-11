package ru.nsu.avdonina.model.parsing.config;

import ru.nsu.avdonina.model.parsing.limits.Limits;

public interface ConfigValidator {
    boolean validate(Config config, Limits limits);
}
