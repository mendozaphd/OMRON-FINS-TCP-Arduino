

enum FinsCommandField : unsigned char
{
    ICF,
    RSC,
    GTC,
    DNA,
    DA1,
    DA2,
    SNA,
    SA1,
    SA2,
    SID,
    MC,
    SC,

    F_PARAM = 12,
};

enum FinsErrors
{
    Completed_normally = 0x0000,
    Service_was_interrupted = 0x0001,
    Local_node_not_part_of_Network = 0x0101,
    Token_timeout_node_number_too_large = 0x0102,
    Number_of_transmit_retries_exceeded = 0x0103,
    Maximum_number_of_frames_exceeded = 0x0104,
    Node_number_setting_error_range_ = 0x0105,
    Node_number_duplication_error = 0x106,
    No_response = 0xFFFF,
    Illegal_SID_error = 0x0FFF,
    Illegal_source_address_error = 0x00FF,
    OUT_OF_RANGE = 0xFFF0,
};

enum CommandType
{
    MEMORY_AREA_READ = 0x0101,
    MEMORY_AREA_WRITE = 0x0102,
    MEMORY_AREA_FILL = 0x0103,
    MULTIPLE_MEMORY_AREA_READ = 0x0104,

    RUN_CMD = 0x0401,
    STOP_CMD = 0x0402,

    CLOCK_READ = 0x0701,
    CLOCK_WRITE = 0x0702,

};

enum MemoryArea : unsigned char
{
    // CIO_Bit = 0x30,
    // WR_Bit = 0x31,
    // HR_Bit = 0x32,
    // AR_Bit = 0x33,
    // CIO_Bit_FS = 0x70,
    // WR_Bit_FS = 0x71,
    // HR_Bit_FS = 0x72,
    CIO = 0xB0,
    WR = 0xB1,
    HR = 0xB2,
    AR = 0xB3,
    // CIO_FS = 0xF0,
    // WR_FS = 0xF1,
    // HR_FS = 0xF2,
    // TIM = 0x09,
    // CNT = 0x09,
    // TIM_FS = 0x49,
    // CNT_FS = 0x49,
    // TIM_PV = 0x89,
    // CNT_PV = 0x89,
    // DM_Bit = 0x02,
    DM = 0x82,
    // TK_Bit = 0x06,
    // TK = 0x46
};

enum FinsCommands
{
    MemoryAreaRead,
    MemoryAreaWrite,
    ControllerDataRead,
    ClockRead,
    ClockWrite,
    StopCPU
};

enum Force_Mode
{
    Forced_Reset = 0x0000 /*Turns OFF (0) the bit/flag and places*/,
    Forced_Set = 0x0001 /*Turns ON (1) the bit/flag and places it*/,
    Forced_Set_reset_OFF_release = 0x8000 /* Turns OFF (0) the bit/flag and releases the forced status. */,
    Forced_set_reset_ON_release = 0x8001 /* Turns ON (1) the bit/flag and releases the forced status */,
    Forced_Set_reset_release = 0xFFFF /* Releases the forced status whileretaining the ON/OFF status*/,
};

enum PLC_MODE
{
    STOP = 0x02FFFF00,
    PROGRAM = 0x02FFFF00,
    RUN = 0x01FFFF04,
    MONITOR = 0x01FFFF02,
    CPU_FAIL = 0x0000000,
};
