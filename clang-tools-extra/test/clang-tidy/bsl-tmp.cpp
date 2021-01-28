// RUN: %check_clang_tidy %s bsl-else-required-after-if %t

namespace bsl
{
    void
    touch() noexcept
    {}
}

void
tests()
{
    int i;
    bool x, y, z;

    if (x) {
    //
        i = 42;
    }

    if (x) {
    //
        if (y) {
        //
            i = 42;
        }
    }

    if (x) {
        if (y) {
        //
            i = 42;
        }
    }
    else {
        i = 0;
    }

    if (x) {
        return;
    }

    if (x) {
    //
        if (y) {
            return;
        }

        bsl::touch();
    }

    if (x) {
    //
        if (y) {
            bsl::touch();
        }
        else {
            return;
        }
    }

    if (x) {
        if (y) {
            //
            return;
        }
    }
    else {
        bsl::touch();
    }

    if (x) {
    //
        if (y) {
            return;
        }
        else {
        //
            return;
        }
    }

    if (x) {
        if (y) {
            return;
        }

        return;
    }

    while (true) {
        if (x) {
        //
            continue;
        }
    }

    while (true) {
        if (x) {
        //
            bsl::touch();
        }
    }

    while (true) {
        if (x) {
        //
            bsl::touch();
        }

        bsl::touch();
    }

    while (true) {
        if (x) {
            continue;
        }

        bsl::touch();
    }

    while (true) {
        if (x) {
            bsl::touch();
        }
        else {
            bsl::touch();
        }
    }

    while (true) {
        if (x) {
        //
            break;
        }
    }

    while (true) {
        if (x) {
        //
            bsl::touch();
        }
    }

    while (true) {
        if (x) {
        //
            bsl::touch();
        }

        bsl::touch();
    }

    while (true) {
        if (x) {
            break;
        }

        bsl::touch();
    }

    while (true) {
        if (x) {
            bsl::touch();
        }
        else {
            bsl::touch();
        }
    }
}
