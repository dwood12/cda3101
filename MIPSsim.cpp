/* On my honor, I have neither given nor received unauthorized aid on this assignment */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <deque>

using namespace std;

// Function Declarations
void getInput(ifstream& input); // function that takes sample.txt
void createOutput_disassembly(ostream& output, const string& instruction, string& instruction_name); // function that output file diassembly.txt
void createOutput_simulator(ostream& output, const string& instruction, string& instruction_name);
void readCategory_1(string& instruction);
void readCategory_2(string& instruction);
void readCategory_3(string& instruction);
string zeroExtend(string& substring, int length);
int binary_to_decimal(int binary, bool negative);
deque<char> decimal_to_binary(int decimal);
int Two_Complement(const char binary[], int length);
void readAllInstructions(const vector<string> instructions);

// Global Variables

int PC = 128;
int registers[32];
int clock_cycle = 1;
const string BREAK = "00010100000000000000000000001101";
ofstream output_disassembly("disassembly.txt");
ofstream output_simulation("simulation.txt");
bool branch = false;
vector<int> data_memory;
int location_of_data;
int address_of_constants;

int main(int argc, char** argv)
{
	for(int i = 0; i < 32; i++)
    {
        registers[i] = 0;
    }

	if(argc != 2)
    {
        cout << "usage: " << argv[0] << " <filename> " << endl;
	}
	ifstream input(argv[1]);
	if(!input.is_open())
    {
        cout << "Could not open file " << argv[1] << endl;
        return 1;
    }

	getInput(input);

    return 0;

}

void getInput(ifstream& input)
{
    string instructions;
    string category_1;
    string category_2;
    string category_3;
    string data;
    vector<string> vec_instructions;
    stringstream constant_output;
    string instruction_name = "";
    stringstream get_constants;
    int length = 0;

    while(getline(input,instructions))
    {
        vec_instructions.push_back(instructions);
    }

    readAllInstructions(vec_instructions); // create disassembly.txt

//    for(int i = location_of_data; i < vec_instructions.size(); i++)
//    {
//        data_memory.push_back(Two_Complement((char*)vec_instructions[i].c_str(), vec_instructions[i].length()));
//    }

    for(int i = 0; i < location_of_data; i++)
    {
        if(branch)
        {
            i = ((PC/4) - 32);
            branch = false;
        }

        if(vec_instructions[i] == BREAK)
        {
            instruction_name = "BREAK";
            createOutput_simulator(output_simulation,"", instruction_name);
            clock_cycle++;
            break;
        }

        else if(vec_instructions[i].substr(0,2) == "00")
        {
            category_1 = vec_instructions[i];
            readCategory_1(category_1);
        }

        else if(vec_instructions[i].substr(0,2) == "01")
        {
            category_2 = vec_instructions[i];
            readCategory_2(category_2);
        }

        else //if(vec_instructions[i].substr(0,2) == "10")
        {
            category_3 = vec_instructions[i];
            readCategory_3(category_3);
        }
    }
}

void readCategory_1(string& instruction)
{
    string rs = instruction.substr(6,5);
    string rt = instruction.substr(11,5);
    int rs_lookup = binary_to_decimal(atoi(rs.c_str()), false);
    int rt_lookup = binary_to_decimal(atoi(rt.c_str()), false);
    string str_instruction_offset = instruction.substr(16,16).append("00");
    string str_instruction_offset_J = instruction.substr(6,26).append("00");
    string str_lw_sw = instruction.substr(16,16);
    int target_offset = binary_to_decimal(atoi(str_instruction_offset.c_str()), false);
    int offset_lw_sw = binary_to_decimal(atoi(str_lw_sw.c_str()), false);
    int target_offset_J = binary_to_decimal(atoi(str_instruction_offset_J.c_str()), false);
    int data_address_instruction_location = 0;

    string instruction_name = "";
    stringstream get_register_num;
    get_register_num << rs_lookup;
    string rs_register = get_register_num.str();
    get_register_num.str("");
    get_register_num.clear();
    get_register_num << rt_lookup;
    string rt_register = get_register_num.str();
    get_register_num.str("");
    get_register_num.clear();
    get_register_num << target_offset;
    string str_target_offset = get_register_num.str();
    get_register_num.str("");
    get_register_num.clear();
    get_register_num << target_offset_J;
    string str_target_offset_J = get_register_num.str();
    get_register_num.str("");
    get_register_num.clear();
    get_register_num << offset_lw_sw;
    string str_offset_lw_sw = get_register_num.str();
    get_register_num.str("");
    get_register_num.clear();

        if(instruction.substr(2,4) == "0101") // BREAK
        {
            instruction_name = "BREAK";
            createOutput_simulator(output_simulation, instruction, instruction_name);
            clock_cycle++;
        }

        else if(instruction.substr(2,4) == "0010") // BEQ
        {
            instruction_name = (string)"BEQ " + (string)"R" + rs_register + (string)", " + (string)"R" + rt_register + (string)", #" + str_target_offset;
            createOutput_simulator(output_simulation, instruction, instruction_name);
            clock_cycle++;

            if(registers[rs_lookup] == registers[rt_lookup])
            {
                PC = PC + 4 + target_offset;
                branch = true;
            }

            else
            {
                PC += 4;
            }
        }

        else if(instruction.substr(2,4) == "0100") // BGTZ
        {
            instruction_name = (string)"BGTZ " + (string)"R" + rs_register + (string)", #" + str_target_offset;
            createOutput_simulator(output_simulation, instruction, instruction_name);
            clock_cycle++;

            if(registers[rs_lookup] > 0)
            {
                PC = (PC + 4 + target_offset);
                branch = true;
            }
            else
            {
               PC+=4;
            }
        }

        else if(instruction.substr(2,4) == "0110") // SW
        {
            offset_lw_sw += registers[rs_lookup];
            data_address_instruction_location = (offset_lw_sw/4) - 32;
            data_memory[data_address_instruction_location-location_of_data] = registers[rt_lookup];
            instruction_name = (string)"SW R" + rt_register + (string)", " + str_offset_lw_sw + (string)"(R" + rs_register + (string)")";
            createOutput_simulator(output_simulation, instruction, instruction_name);
            PC+=4;
            clock_cycle++;
        }

        else if(instruction.substr(2,4) == "0111") // LW
        {
            offset_lw_sw += registers[rs_lookup];
            data_address_instruction_location = (offset_lw_sw/4) - 32;
            registers[rt_lookup] = data_memory[data_address_instruction_location-location_of_data];
            instruction_name = (string)"LW R" + rt_register + (string)", " + str_offset_lw_sw + (string)"(R" + rs_register + (string)")";
            createOutput_simulator(output_simulation, instruction, instruction_name);
            PC+=4;
            clock_cycle++;
        }

        else
        {
            instruction_name = (string)"J #" + str_target_offset_J;
            createOutput_simulator(output_simulation, instruction, instruction_name);
            PC = target_offset_J;
            branch = true;
            clock_cycle++;
        }
}

void readCategory_2(string& instruction)
{
    string instruction_name = "";
    string rs = instruction.substr(2,5);
    string rt = instruction.substr(7,5);
    string rd = instruction.substr(16,5);
    int rs_lookup = binary_to_decimal(atoi(rs.c_str()), false);
    int rt_lookup = binary_to_decimal(atoi(rt.c_str()), false);
    int rd_lookup = binary_to_decimal(atoi(rd.c_str()), false);
    stringstream get_register_num;
    get_register_num << rs_lookup;
    string rs_register = get_register_num.str();
    get_register_num.str("");
    get_register_num.clear();
    get_register_num << rt_lookup;
    string rt_register = get_register_num.str();
    get_register_num.str("");
    get_register_num.clear();
    get_register_num << rd_lookup;
    string rd_register = get_register_num.str();
    get_register_num.str("");
    get_register_num.clear();
    deque<char> rs_logical = decimal_to_binary(registers[rs_lookup]);
    deque<char> rt_logical = decimal_to_binary(registers[rt_lookup]);
    deque<char> rd_logical;
    string converter = "";

    if(instruction.substr(12,4) == "0000") // ADD
    {
        instruction_name = (string)"ADD R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
        registers[rd_lookup] = registers[rs_lookup] + registers[rt_lookup];
        createOutput_simulator(output_simulation, instruction, instruction_name);
        clock_cycle++;
        PC += 4;
    }

    if(instruction.substr(12,4) == "0001") // SUB
    {
        instruction_name = (string)"SUB R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
        registers[rd_lookup] = registers[rs_lookup] - registers[rt_lookup];
        createOutput_simulator(output_simulation, instruction, instruction_name);
        clock_cycle++;
        PC += 4;
    }

    if(instruction.substr(12,4) == "0010") // MUL
    {
        instruction_name = (string)"MUL R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
        registers[rd_lookup] = registers[rs_lookup] * registers[rt_lookup];
        createOutput_simulator(output_simulation, instruction, instruction_name);
        clock_cycle++;
        PC += 4;
    }

    if(instruction.substr(12,4) == "0011") // AND
    {
        instruction_name = (string)"AND R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
        for(int j = 0; j < 32; j++)
        {
            if((rt_logical[j]-'0') && (rs_logical[j]-'0'))
               {
                    rd_logical.push_back('1');
               }
            else
                {
                    rd_logical.push_back('0');
                }
        }

        for(int k = 0; k < rd_logical.size(); k++)
        {
            converter += rd_logical[k];
        }

        registers[rd_lookup] = binary_to_decimal(atoi((char*) converter.c_str()), false);
        createOutput_simulator(output_simulation, instruction, instruction_name);
        clock_cycle++;
        PC+=4;
    }

    if(instruction.substr(12,4) == "0100") // OR
    {
        instruction_name = (string)"OR R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
        for(int j = 0; j < 32; j++)
                {
                    if((rt_logical[j]-'0') || (rs_logical[j]-'0'))
                       {
                            rd_logical.push_back('1');
                       }
                    else
                        {
                            rd_logical.push_back('0');
                        }
                }

        for(int k = 0; k < rd_logical.size(); k++)
        {
            converter += rd_logical[k];
        }

        registers[rd_lookup] = binary_to_decimal(atoi((char*)converter.c_str()), false);
        createOutput_simulator(output_simulation, instruction, instruction_name);
        clock_cycle++;
        PC+=4;
    }

    if(instruction.substr(12,4) == "0101") // XOR
    {
        instruction_name = (string)"XOR R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
        for(int j = 0; j < 32; j++)
        {
            if(((rt_logical[j]-'0') || (rs_logical[j]-'0')) && !((rt_logical[j]-'0') && (rs_logical[j]-'0')))
               {
                    rd_logical.push_back('1');
               }
            else
                {
                    rd_logical.push_back('0');
                }
        }

        for(int k = 0; k < rd_logical.size(); k++)
        {
            converter += rd_logical[k];
        }

        registers[rd_lookup] = binary_to_decimal(atoi((char*)converter.c_str()), false);
        createOutput_simulator(output_simulation, instruction, instruction_name);
        clock_cycle++;
        PC+=4;
    }

    if(instruction.substr(12,4) == "0110") // NOR
    {
        instruction_name = (string)"NOR R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
        for(int j = 0; j < 32; j++)
        {
            if(!((rt_logical[j]-'0') || (rs_logical[j]-'0')))
               {
                    rd_logical.push_back('1');
               }
            else
                {
                    rd_logical.push_back('0');
                }
        }

        for(int k = 0; k < rd_logical.size(); k++)
        {
            converter += rd_logical[k];
        }

        registers[rd_lookup] = Two_Complement((char*)converter.c_str(), converter.length());
        createOutput_simulator(output_simulation, instruction, instruction_name);
        clock_cycle++;
        PC+=4;
    }
}

void readCategory_3(string& instruction)
{
    string instruction_name = "";
    string rs = instruction.substr(2,5);
    string rt = instruction.substr(7,5);
    int rs_lookup = binary_to_decimal(atoi(rs.c_str()), false);
    int rt_lookup = binary_to_decimal(atoi(rt.c_str()), false);
    string str_immediate = instruction.substr(16,16);
    str_immediate = zeroExtend(str_immediate, str_immediate.length());
    int immediate_value = binary_to_decimal(atoi(str_immediate.c_str()), false);
    stringstream get_register_num;
    get_register_num << rs_lookup;
    string rs_register = get_register_num.str();
    get_register_num.str("");
    get_register_num.clear();
    get_register_num << rt_lookup;
    string rt_register = get_register_num.str();
    get_register_num.str("");
    get_register_num.clear();
    get_register_num << immediate_value;
    string immediate_value_output = get_register_num.str();
    get_register_num.str("");
    get_register_num.clear();

    deque<char> rs_logical = decimal_to_binary(registers[rs_lookup]);
    deque<char> rt_logical;
    string converter = "";

    if(instruction.substr(12,4) == "0000") // op-code ADDI
    {
        registers[rt_lookup] = registers[rs_lookup] + immediate_value;
        instruction_name = (string)"ADDI R" + rt_register + (string)", R" + rs_register + (string)", #" + immediate_value_output;
        createOutput_simulator(output_simulation, instruction, instruction_name);
        clock_cycle++;
        PC += 4;

    }

    if(instruction.substr(12,4) == "0001")// op-code ANDI
    {
        instruction_name = (string) "ANDI R" + rt_register + (string)", R" + rs_register + (string)", #" + immediate_value_output;
        for(int j = 0; j < 32; j++)
        {
            if(rs_logical[j]-'0' && str_immediate[j] - '0')
            {
                rt_logical.push_back('1');
            }
            else
            {
                rt_logical.push_back('0');
            }
        }

        for(int k = 0; k < rt_logical.size(); k++)
        {
            converter += rt_logical[k];
        }

        registers[rt_lookup] = Two_Complement((char*)converter.c_str(), converter.length());
        createOutput_simulator(output_simulation, instruction, instruction_name);
        clock_cycle++;
        PC += 4;
    }

    if(instruction.substr(12,4) == "0010") // op-code ORI
    {
        instruction_name = (string) "ORI R" + rt_register + (string)", R" + rs_register + (string)", #" + immediate_value_output;
        for(int j = 0; j < 32; j++)
        {
            if(rs_logical[j]-'0' || str_immediate[j] - '0')
            {
                rt_logical.push_back('1');
            }
            else
            {
                rt_logical.push_back('0');
            }
        }

        for(int k = 0; k < rt_logical.size(); k++)
        {
            converter += rt_logical[k];
        }

        registers[rt_lookup] = Two_Complement((char*)converter.c_str(), converter.length());
        createOutput_simulator(output_simulation, instruction, instruction_name);
        clock_cycle++;
        PC += 4;
    }

    if(instruction.substr(12,4) == "0011") // op-code XORI
    {
        instruction_name = (string) "XORI R" + rt_register + (string)", R" + rs_register + (string)", #" + immediate_value_output;
        for(int j = 0; j < 32; j++)
        {
            if(((rs_logical[j]-'0') || (str_immediate[j]-'0')) && !((rs_logical[j]-'0') && (str_immediate[j]-'0')))
            {
                rt_logical.push_back('1');
            }
            else
            {
                rt_logical.push_back('0');
            }
        }

        for(int k = 0; k < rt_logical.size(); k++)
        {
            converter += rt_logical[k];
        }
        registers[rt_lookup] = Two_Complement((char*)converter.c_str(), converter.length());
        createOutput_simulator(output_simulation, instruction, instruction_name);
        clock_cycle++;
        PC += 4;
    }
}

string zeroExtend(string& substring, int length)
{
    string sign_extended_string = "";
    for(int i = 0; i < 32 - length; i++)
    {
        sign_extended_string += (string)"0";
    }
    sign_extended_string += substring;
    return sign_extended_string;
}

void createOutput_disassembly(ostream& output, const string& instruction, string& instruction_name)
{
    output << instruction << '\t' << PC << '\t' << instruction_name << endl;
}

void createOutput_simulator(ostream& output, const string& instruction, string& instruction_name)
{
    output << "--------------------" << '\n' << "Cycle:" << clock_cycle << '\t' << PC << '\t' << instruction_name << '\n' << endl;
    output << "Registers" << endl << "R00:";

    for(int i = 0; i < 8; i++)
    {
        if(i == 0)
        {
            output << '\t' << registers[i] << '\t';
        }

        else
        {
            output << registers[i] << '\t';
        }
    }

    output << endl << "R08:";

    for(int i = 8; i < 16; i++)
    {
        if(i == 8)
        {
            output << '\t' << registers[i] << '\t';
        }
        else
        {
        output << registers[i] << '\t';
        }
    }
    output << endl << "R16:";
    for(int i = 16; i < 24; i++)
    {
        if(i == 16)
        {
            output << '\t' << registers[i] << '\t';
        }

        else
        {
            output << registers[i] << '\t';
        }
    }

    output << endl << "R24:";
    for(int i = 24; i < 32; i++)
    {
        if(i == 24)
        {
            output << '\t' << registers[i] << '\t';
        }
        else
        {
        output << registers[i] << '\t';
        }
    }
    output << endl << endl << "Data" << endl;
    int k = 0;
    int addresses = address_of_constants;

    for(int i = 0; i < data_memory.size()/8; i++)
    {
        output << addresses << ":";
        for(int j = 0; j < 8 && k < data_memory.size(); j++)
        {
            if(j == 0)
            {
                output << '\t' << data_memory[k] << '\t';
            }
            else
            {
                output << data_memory[k] << '\t';
            }

            k++;
            addresses+=4;
        }
        output << endl;
    }
    output << endl;

}

int binary_to_decimal(int binary, bool negative)
{
    int decimal = 0;
    int remainder= 1;
    int factor = 1;

    while(binary > 0)
    {
        remainder = binary % 10;
        decimal = decimal + remainder*factor;
        factor *= 2;
        binary /= 10;
    }

    return decimal;
}

deque<char> decimal_to_binary(int decimal)
{
    deque<char> binary_value;
    int remainder;
    int sign_extend;
    while(decimal > 0)
    {
        remainder = decimal % 2;
        decimal /= 2;
        binary_value.push_front(remainder + '0');
    }

    sign_extend = binary_value.size();

    for(int i = 0; i < (32 - sign_extend); i++)
    {
        binary_value.push_front('0');
    }
    return binary_value;
}

int Two_Complement(const char binary[], int length)
{
    int bits = length;
    int value = 0;
    bool negative = false;
    char* binary_copy = new char[bits];

    for(int i = 0; i < bits; i++)
    {
        binary_copy[i] = binary[i];
    }

    if(binary_copy[0] == '0')
    {
        return binary_to_decimal(atoi((char*)binary_copy), false);
    }

    else
    {
        for(int i = 0; i < bits - 1; i++)
        {
            if(binary_copy[i] == '0') // invert bits: 0->1 and 1->0
            {
                binary_copy[i] = '1';
            }

            else //binary[i] == '1'&& negative == true)
            {
                binary_copy[i] = '0';
            }
        }
    return (-(binary_to_decimal(atoi((char*)binary_copy), false)) - 1);
    }

}

void readAllInstructions(const vector<string> instructions)
{
    string instruction_name = "";
    int location_of_constants = 0;
    for(int i = 0; i < instructions.size(); i++)
    {
        if(instructions[i].substr(0,2) == "00") // Category 1
        {
            string rs = instructions[i].substr(6,5);
            string rt = instructions[i].substr(11,5);
            int rs_lookup = binary_to_decimal(atoi(rs.c_str()), false);
            int rt_lookup = binary_to_decimal(atoi(rt.c_str()), false);
            string str_instruction_offset = instructions[i].substr(16,16).append("00");
            string str_instruction_offset_J = instructions[i].substr(6,26).append("00");
            string str_lw_sw = instructions[i].substr(16,16);
            int target_offset = binary_to_decimal(atoi(str_instruction_offset.c_str()), false);
            int offset_lw_sw = Two_Complement(str_lw_sw.c_str(), str_lw_sw.length());
            int target_offset_J = Two_Complement(str_instruction_offset_J.c_str(), str_instruction_offset_J.length());

            string instruction_name = "";
            stringstream get_register_num;
            get_register_num << rs_lookup;
            string rs_register = get_register_num.str();
            get_register_num.str("");
            get_register_num.clear();
            get_register_num << rt_lookup;
            string rt_register = get_register_num.str();
            get_register_num.str("");
            get_register_num.clear();
            get_register_num << target_offset;
            string str_target_offset = get_register_num.str();
            get_register_num.str("");
            get_register_num.clear();
            get_register_num << offset_lw_sw;
            string str_offset_lw_sw = get_register_num.str();
            get_register_num.str("");
            get_register_num.clear();
            get_register_num << target_offset_J;
            string str_target_offset_J = get_register_num.str();
            get_register_num.str("");
            get_register_num.clear();


            if(instructions[i].substr(2,4) == "0000") // J
            {
                instruction_name = (string)"J #" + str_target_offset_J;
                createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                PC+=4;
            }

            else if(instructions[i].substr(2,4) == "0010") // BEQ
            {
                instruction_name = (string)"BEQ " + (string)"R" + rs_register + (string)", " + (string)"R" + rt_register + (string)", #" + str_target_offset;
                createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                PC+=4;
            }

            else if(instructions[i].substr(2,4) == "0100") // BGTZ
            {
                instruction_name = (string)"BGTZ " + (string)"R" + rs_register + (string)", #" + str_target_offset;
                createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                PC+=4;
            }

            else if(instructions[i].substr(2,4) == "0101") // BREAK
            {
                instruction_name = "BREAK";
                createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                PC+=4;
                location_of_constants = i + 1;
                location_of_data = location_of_constants;
                address_of_constants = PC;
                break;
            }

            else if(instructions[i].substr(2,4) == "0110") // SW
            {
                instruction_name = (string)"SW R" + rt_register + (string)", " + str_offset_lw_sw + (string)"(R" + rs_register + (string)")";
                createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                PC+=4;
            }
            else //if(instructions[i].substr(2,4) == "0111") // LW
            {
                instruction_name = (string)"LW R" + rt_register + (string)", " + str_offset_lw_sw + (string)"(R" + rs_register + (string)")";
                createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                PC+=4;
            }
    }

        else if(instructions[i].substr(0,2) == "01") // Category 2
        {
            string rs = instructions[i].substr(2,5);
            string rt = instructions[i].substr(7,5);
            string rd = instructions[i].substr(16,5);
            int rs_lookup = binary_to_decimal(atoi(rs.c_str()), false);
            int rt_lookup = binary_to_decimal(atoi(rt.c_str()), false);
            int rd_lookup = binary_to_decimal(atoi(rd.c_str()), false);
            stringstream get_register_num;
            get_register_num << rs_lookup;
            string rs_register = get_register_num.str();
            get_register_num.str("");
            get_register_num.clear();
            get_register_num << rt_lookup;
            string rt_register = get_register_num.str();
            get_register_num.str("");
            get_register_num.clear();
            get_register_num << rd_lookup;
            string rd_register = get_register_num.str();
            get_register_num.str("");
            get_register_num.clear();

            if(instructions[i].substr(12,4) == "0000") // ADD
                {
                    instruction_name = (string)"ADD R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
                    createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                    PC += 4;
                }

            else if(instructions[i].substr(12,4) == "0001") // SUB
                {
                    instruction_name = (string)"SUB R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
                    createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                    PC += 4;
                }

            else if(instructions[i].substr(12,4) == "0010") // MUL
                {
                    instruction_name = (string)"MUL R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
                    createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                    PC += 4;
                }

             else if(instructions[i].substr(12,4) == "0011") // AND
                {
                    instruction_name = (string)"AND R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
                    createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                    PC+=4;
                }

            else if(instructions[i].substr(12,4) == "0100") // OR
                {
                    instruction_name = (string)"OR R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
                    createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                    PC+=4;
                }

            else if(instructions[i].substr(12,4) == "0101") // XOR
                {
                    instruction_name = (string)"XOR R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
                    createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                    PC+=4;
                }

            else //if(instructions[i].substr(12,4) == "0110") // NOR
                {
                    instruction_name = (string)"NOR R" + rd_register + (string)", R" + rs_register + (string)", R" + rt_register;
                    createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                    PC+=4;
                }
        }

        else //if(instructions[i].substr(0,2) == "10")
        {
            string rs = instructions[i].substr(2,5);
            string rt = instructions[i].substr(7,5);
            int rs_lookup = binary_to_decimal(atoi(rs.c_str()), false);
            int rt_lookup = binary_to_decimal(atoi(rt.c_str()), false);
            string str_immediate = instructions[i].substr(16,16);
            int immediate_value = Two_Complement(str_immediate.c_str(), str_immediate.length());
            stringstream get_register_num;
            get_register_num << rs_lookup;
            string rs_register = get_register_num.str();
            get_register_num.str("");
            get_register_num.clear();
            get_register_num << rt_lookup;
            string rt_register = get_register_num.str();
            get_register_num.str("");
            get_register_num.clear();
            get_register_num << immediate_value;
            string immediate_value_output = get_register_num.str();
            get_register_num.str("");
            get_register_num.clear();

            if(instructions[i].substr(12,4) == "0000") // ADDI
            {
                    instruction_name = (string)"ADDI " + (string)"R" + rt_register + (string)", R" + rs_register + (string)", #" + immediate_value_output;
                    createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                    PC += 4;
            }
            else if(instructions[i].substr(12,4) == "0001") // ANDI
            {
                    instruction_name = (string)"ANDI " + (string)"R" + rt_register + (string)", R" + rs_register + (string)", #" + immediate_value_output;
                    createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                    PC += 4;
            }

            else if(instructions[i].substr(12,4) == "0010") // ORI
            {
                    instruction_name = (string)"ORI " + (string)"R" + rt_register + (string)", R" + rs_register + (string)", #" + immediate_value_output;
                    createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                    PC += 4;
            }

            else //if(instructions[i].substr(12,4) == "0011") // XORI
            {
                    instruction_name = (string)"XORI " + (string)"R" + rt_register + (string)", R" + rs_register + (string)", #" + immediate_value_output;
                    createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
                    PC += 4;
            }

        }

    }

    int data_memory_value = 0;
    stringstream str_data_memory;
    int length = 0;

    for(int i = location_of_constants; i < instructions.size(); i++)
    {
        length = instructions[i].length();
        data_memory_value = Two_Complement((char*)instructions[i].c_str(), length);
        data_memory.push_back(data_memory_value);
        str_data_memory << data_memory_value;
        instruction_name = str_data_memory.str() + '\t';
        str_data_memory.str("");
        str_data_memory.clear();
        createOutput_disassembly(output_disassembly, instructions[i], instruction_name);
        PC += 4;
    }

    PC = 128; // reset PC for simulator upon function return
}
