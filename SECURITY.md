# Security

Do not commit Wi-Fi credentials or device-specific keys. Start from
`credentials.example.h` and keep the resulting `credentials.h` file local.

If a credential is committed accidentally, revoke or rotate it before removing it from
the repository.
