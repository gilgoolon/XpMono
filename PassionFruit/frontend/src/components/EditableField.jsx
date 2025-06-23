import React, { useState } from 'react';
import {
    IconButton,
    TextField,
    Typography,
    Box
} from '@mui/material';
import EditIcon from '@mui/icons-material/Edit';
import CheckIcon from '@mui/icons-material/Check';
import CloseIcon from '@mui/icons-material/Close';

export default function EditableField({ value, onSave, label }) {
    const [editing, setEditing] = useState(false);
    const [tempValue, setTempValue] = useState(value ?? '');

    const handleEdit = () => {
        setTempValue(value ?? '');
        setEditing(true);
    };

    const handleCancel = () => {
        setEditing(false);
        setTempValue(value ?? '');
    };

    const handleConfirm = () => {
        if (tempValue !== value) {
            onSave(tempValue === '' ? null : tempValue);
        }
        setEditing(false);
    };

    const displayText = value ?? label;

    return (
        <Box display="flex" alignItems="center" gap={1}>
            {editing ? (
                <>
                    <TextField
                        label={label}
                        size="small"
                        value={tempValue}
                        onChange={(e) => setTempValue(e.target.value)}
                        autoFocus
                    />
                    <IconButton onClick={handleConfirm} color="success">
                        <CheckIcon />
                    </IconButton>
                    <IconButton onClick={handleCancel} color="error">
                        <CloseIcon />
                    </IconButton>
                </>
            ) : (
                <>
                    <Typography
                        variant="body1"
                        sx={{ color: value == null ? 'gray' : 'text.primary' }}
                    >
                        {displayText}
                    </Typography>
                    <IconButton onClick={handleEdit} size="small">
                        <EditIcon fontSize="small" />
                    </IconButton>
                </>
            )}
        </Box>
    );
}
