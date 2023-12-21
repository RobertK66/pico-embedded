#pragma once
/**
 * Copyright (c) 2022 RKOS-IT
 */

// Module API
void CliInit(void);
void CliMain(void);

// With this function you can register your custom command handler(s)
void CliRegisterCommand(char* cmdStr, void (*callback)(int argc, char* argv[]));