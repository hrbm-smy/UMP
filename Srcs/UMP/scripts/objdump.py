Import("env")

env.SetDefault(OBJDUMP="arm-none-eabi-objdump")

# Track the dump as a build output so it is regenerated after clean,
# an ELF change, or removal of the dump alone.
dump = env.Command(
    "$BUILD_DIR/${PROGNAME}.objdump.txt",
    "$PROGPATH",
    env.VerboseAction(
        '"$OBJDUMP" -d -S -C "$SOURCE" > "$TARGET"',
        "Generating $TARGET",
    ),
)
env.Depends("buildprog", dump)
env.Default(dump)
