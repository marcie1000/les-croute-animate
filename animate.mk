##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=animate
ConfigurationName      :=Debug
WorkspaceConfiguration :=Debug
WorkspacePath          :=/home/adelia/Programmation/CodeLite/les_croute
ProjectPath            :=/home/adelia/Programmation/CodeLite/les_croute/animate
IntermediateDirectory  :=../build-$(WorkspaceConfiguration)/animate
OutDir                 :=$(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Adelia
Date                   :=15/03/23
CodeLitePath           :=/home/adelia/.codelite
MakeDirCommand         :=mkdir -p
LinkerName             :=/usr/bin/clang++-15
SharedObjectLinkerName :=/usr/bin/clang++-15 -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputDirectory        :=/home/adelia/Programmation/CodeLite/les_croute/build-$(WorkspaceConfiguration)/bin
OutputFile             :=../build-$(WorkspaceConfiguration)/bin/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :=$(IntermediateDirectory)/ObjectsList.txt
PCHCompileFlags        :=
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)SDL2 $(LibrarySwitch)SDL2_ttf $(LibrarySwitch)SDL2_image $(LibrarySwitch)SDL2_mixer $(LibrarySwitch)m 
ArLibs                 :=  "SDL2" "SDL2_ttf" "SDL2_image" "SDL2_mixer" "m" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := /usr/bin/ar
CXX      := /usr/bin/clang++-15
CC       := /usr/bin/clang-15
CXXFLAGS :=  -gdwarf-2 -O0 -Wall $(Preprocessors)
CFLAGS   := -std=c17 -O0 -gdwarf-2 -pedantic -std=c17 -Wall -fno-common  -fno-builtin $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/fichiers.c$(ObjectSuffix) $(IntermediateDirectory)/collisions.c$(ObjectSuffix) $(IntermediateDirectory)/anim.c$(ObjectSuffix) $(IntermediateDirectory)/textures_fx.c$(ObjectSuffix) $(IntermediateDirectory)/npc.c$(ObjectSuffix) $(IntermediateDirectory)/audio.c$(ObjectSuffix) $(IntermediateDirectory)/main.c$(ObjectSuffix) $(IntermediateDirectory)/fx_switch_events.c$(ObjectSuffix) $(IntermediateDirectory)/sdl_init_and_quit.c$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: MakeIntermediateDirs $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) "$(IntermediateDirectory)"
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@$(MakeDirCommand) "$(IntermediateDirectory)"
	@$(MakeDirCommand) "$(OutputDirectory)"

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "$(IntermediateDirectory)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/fichiers.c$(ObjectSuffix): fichiers.c $(IntermediateDirectory)/fichiers.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/adelia/Programmation/CodeLite/les_croute/animate/fichiers.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fichiers.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/fichiers.c$(DependSuffix): fichiers.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fichiers.c$(ObjectSuffix) -MF$(IntermediateDirectory)/fichiers.c$(DependSuffix) -MM fichiers.c

$(IntermediateDirectory)/fichiers.c$(PreprocessSuffix): fichiers.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/fichiers.c$(PreprocessSuffix) fichiers.c

$(IntermediateDirectory)/collisions.c$(ObjectSuffix): collisions.c $(IntermediateDirectory)/collisions.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/adelia/Programmation/CodeLite/les_croute/animate/collisions.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/collisions.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/collisions.c$(DependSuffix): collisions.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/collisions.c$(ObjectSuffix) -MF$(IntermediateDirectory)/collisions.c$(DependSuffix) -MM collisions.c

$(IntermediateDirectory)/collisions.c$(PreprocessSuffix): collisions.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/collisions.c$(PreprocessSuffix) collisions.c

$(IntermediateDirectory)/anim.c$(ObjectSuffix): anim.c $(IntermediateDirectory)/anim.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/adelia/Programmation/CodeLite/les_croute/animate/anim.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/anim.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/anim.c$(DependSuffix): anim.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/anim.c$(ObjectSuffix) -MF$(IntermediateDirectory)/anim.c$(DependSuffix) -MM anim.c

$(IntermediateDirectory)/anim.c$(PreprocessSuffix): anim.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/anim.c$(PreprocessSuffix) anim.c

$(IntermediateDirectory)/textures_fx.c$(ObjectSuffix): textures_fx.c $(IntermediateDirectory)/textures_fx.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/adelia/Programmation/CodeLite/les_croute/animate/textures_fx.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/textures_fx.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/textures_fx.c$(DependSuffix): textures_fx.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/textures_fx.c$(ObjectSuffix) -MF$(IntermediateDirectory)/textures_fx.c$(DependSuffix) -MM textures_fx.c

$(IntermediateDirectory)/textures_fx.c$(PreprocessSuffix): textures_fx.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/textures_fx.c$(PreprocessSuffix) textures_fx.c

$(IntermediateDirectory)/npc.c$(ObjectSuffix): npc.c $(IntermediateDirectory)/npc.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/adelia/Programmation/CodeLite/les_croute/animate/npc.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/npc.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/npc.c$(DependSuffix): npc.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/npc.c$(ObjectSuffix) -MF$(IntermediateDirectory)/npc.c$(DependSuffix) -MM npc.c

$(IntermediateDirectory)/npc.c$(PreprocessSuffix): npc.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/npc.c$(PreprocessSuffix) npc.c

$(IntermediateDirectory)/audio.c$(ObjectSuffix): audio.c $(IntermediateDirectory)/audio.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/adelia/Programmation/CodeLite/les_croute/animate/audio.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/audio.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/audio.c$(DependSuffix): audio.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/audio.c$(ObjectSuffix) -MF$(IntermediateDirectory)/audio.c$(DependSuffix) -MM audio.c

$(IntermediateDirectory)/audio.c$(PreprocessSuffix): audio.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/audio.c$(PreprocessSuffix) audio.c

$(IntermediateDirectory)/main.c$(ObjectSuffix): main.c $(IntermediateDirectory)/main.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/adelia/Programmation/CodeLite/les_croute/animate/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.c$(DependSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.c$(ObjectSuffix) -MF$(IntermediateDirectory)/main.c$(DependSuffix) -MM main.c

$(IntermediateDirectory)/main.c$(PreprocessSuffix): main.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.c$(PreprocessSuffix) main.c

$(IntermediateDirectory)/fx_switch_events.c$(ObjectSuffix): fx_switch_events.c $(IntermediateDirectory)/fx_switch_events.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/adelia/Programmation/CodeLite/les_croute/animate/fx_switch_events.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/fx_switch_events.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/fx_switch_events.c$(DependSuffix): fx_switch_events.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/fx_switch_events.c$(ObjectSuffix) -MF$(IntermediateDirectory)/fx_switch_events.c$(DependSuffix) -MM fx_switch_events.c

$(IntermediateDirectory)/fx_switch_events.c$(PreprocessSuffix): fx_switch_events.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/fx_switch_events.c$(PreprocessSuffix) fx_switch_events.c

$(IntermediateDirectory)/sdl_init_and_quit.c$(ObjectSuffix): sdl_init_and_quit.c $(IntermediateDirectory)/sdl_init_and_quit.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/adelia/Programmation/CodeLite/les_croute/animate/sdl_init_and_quit.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/sdl_init_and_quit.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/sdl_init_and_quit.c$(DependSuffix): sdl_init_and_quit.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/sdl_init_and_quit.c$(ObjectSuffix) -MF$(IntermediateDirectory)/sdl_init_and_quit.c$(DependSuffix) -MM sdl_init_and_quit.c

$(IntermediateDirectory)/sdl_init_and_quit.c$(PreprocessSuffix): sdl_init_and_quit.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/sdl_init_and_quit.c$(PreprocessSuffix) sdl_init_and_quit.c


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


