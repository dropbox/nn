# `nn`: Non-nullable pointers for C++

`nn` is a type that helps you enforce at compile time the contract that a given pointer
can't be null. It wraps around raw pointers or smart pointers, and works particularly
well out of the box with `std::unique_ptr` and `std::shared_ptr`.

Here's an example:

    class Widget : public WidgetBase {
    public:
      Widget(nn_shared_ptr<Gadget> gadget) : m_gadget(move(gadget)) {}
      // ...
    private:
      nn_shared_ptr<Gadget> m_gadget;
    };

    // nn_make_unique and nn_make_shared always return non-null values
    nn_unique_ptr<Widget> my_widget = nn_make_unique<Widget>(nn_make_shared<Gadget>());

    // but what if we have a pointer already and we don't know if it's null?
    shared_ptr<Gadget> this_might_be_null = ...
    my_widget = nn_make_unique<Widget>(NN_CHECK_ASSERT(this_might_be_null));

    // implicit nn_unique_ptr -> nn_shared_ptr works just like unique_ptr -> shared_ptr
    nn_shared_ptr<Widget> shared_widget = move(my_widget);

    // the `nn` implicitly casts away if needed
    void save_ownership_somewhere(shared_ptr<Widget>);
    save_ownership_somewhere(shared_widget);

    // implicit upcasts work too
    nn_shared_ptr<WidgetBase> base_ptr = shared_widget;

Compile-time checking helps find bugs sooner. At Dropbox we use `nn` pervasively in our
cross-platform C++ codebase.
