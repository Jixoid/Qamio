# Qamio

**Qamio** is a modular, extensible, and open-source operating environment, designed as the core execution layer for QAOS. It brings a secure and scalable architecture with a component-based kernel and a flexible user-space layout, aiming to serve as a modern base for embedded, desktop, and experimental computing platforms.


## ✨ Key Features

- **Modular Design**: Every system component is built as an independent package. Qamio supports hot deployment, fine-grained access control, and dynamic service composition.
- **Layered Architecture**: Clean separation between system, vendor, product, and user layers — fully Treble-style.
- **Secure Execution**: Controlled module loading, capability-based permissions, and a trusted module system.
- **Minimal Core Kernel**: The `com.qaos.kernel` module only handles loading and delegation — everything else is a pluggable component.


## 📜 Licensing

This project uses **multiple open-source licenses**.

Each file contains a header specifying the applicable license.
Users and contributors are encouraged to review these licenses carefully.

- GPL for **source code**.
- LGPL for **build system**.
- CC BY-SA for **documentation**.

Please see the `LICENSE` file for detailed information.
You can find the license texts in the `LICENSES` folder.


## 💬 Contributing

Qamio is a community-driven project. Contributions are welcome, whether in the form of:
- New Nuc modules
- Bug reports
- Design ideas
- Documentation

```
  Fork the repository

  Create a new branch (e.g., feature/my-new-thing)

  Make your changes and commit them

  Submit a pull request
```


### QAOS

Built with ❤️ by the open source community.
